
#include <algorithm.h>


namespace X_ROOT_NS { namespace algorithm {


    ////////// ////////// ////////// ////////// //////////

    void quick_copy(void * dst, const void * src, size_t size) _NE
    {
        uint64_t * dst1 = (uint64_t *)dst;
        const uint64_t * src1 = (const uint64_t *)src;

        while (size > sizeof(uint64_t))
        {
            *dst1++ = *src1++;
            size -= sizeof(uint64_t);
        }

        switch (size)
        {
            #define __Case(_size)                           \
                case _size:                                 \
                    al::quick_copy<_size>(dst1, src1);      \
                    break;

            __Case(1)
            __Case(2)
            __Case(3)
            __Case(4)
            __Case(5)
            __Case(6)
            __Case(7)
            __Case(8)

            #undef __Case
        }
    }

    ////////// ////////// ////////// ////////// //////////

    void quick_zero(void * p, size_t size) _NE
    {
        uint64_t * p1 = (uint64_t *)p;

        while (size > sizeof(uint64_t))
        {
            *p1++ = 0;
            size -= sizeof(uint64_t);
        }

        switch (size)
        {
            #define __Case(_size)                           \
                case _size:                                 \
                    al::quick_zero<_size>(p1);              \
                    break;

            __Case(1)
            __Case(2)
            __Case(3)
            __Case(4)
            __Case(5)
            __Case(6)
            __Case(7)
            __Case(8)

            #undef __Case
        }
    }

    ////////// ////////// ////////// ////////// //////////


} }  // X_ROOT_NS::algorithm
