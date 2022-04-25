#include <rtlib.h>
#include <mm.h>

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

    // Tuple arguments
    struct __tuple_args_t
    {
        robject_t       tuple;
        rt_type_t *     type;
    };

    ////////// ////////// ////////// ////////// //////////

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
        _A(rt_type != nullptr);
        _A(rt_type->get_kind() == rt_type_kind_t::generic);

        int position = 0;
        byte_t * ptr = (byte_t *)tuple;
        rt_stack_unit_t * argument = (rt_stack_unit_t *)*args->arguments;

        // Each all tuple fields.
        rt_type->each_field(ctx.analyzer.env, [&](ref_t ref, rt_field_base_t * field) {

            rt_type_t * field_type = field->get_field_type(ctx.analyzer);
            msize_t offset = rt_type->get_field_offset(ctx.analyzer.env, position);
            msize_t size = field_type->get_variable_size(ctx.analyzer.env);

            set_variable_value(ptr + offset, argument, size);
            argument += unit_size_of(size);

            position++;
            return true;
        });
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
    struct __tuple_get_count_args_t : __tuple_args_t
    {
        // empty.
    };

    void tuple_get_count(rtlib_context_t & ctx)
    {
        typedef __tuple_get_count_args_t args_t;
        args_t * args = ctx.args<args_t>();
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib

