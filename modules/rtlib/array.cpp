#include <rtlib.h>
#include <mm.h>

namespace X_ROOT_NS { namespace modules { namespace rtlib {

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
        robject_t   tuple;
    };


    ////////// ////////// ////////// ////////// //////////

	// Tuple_SetRange
	struct __tuple_set_range_args_t : __tuple_args_t
	{
		rptr_t	args;
	};

	void tuple_set_range(rtlib_context_t & ctx)
	{
		typedef __tuple_set_range_args_t args_t;
		args_t * args = ctx.args<args_t>();
	}

    ////////// ////////// ////////// ////////// //////////

	// Tuple_Get
	struct __tuple_get_args_t : __tuple_args_t
	{
		rint32_t	index;
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

} } }
