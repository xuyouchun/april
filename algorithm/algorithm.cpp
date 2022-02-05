
#include <algorithm.h>

namespace X_ROOT_NS::algorithm {


    ////////// ////////// ////////// ////////// //////////

    X_ALWAYS_INLINE void __quick_copy(void * dst, const void * src, size_t size) _NE
    {
        switch (size)
        {
            #define __Case(_size)                                                       \
                case _size:                                                             \
                    al::quick_copy<_size>(dst, src);                                    \
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

    X_ALWAYS_INLINE void __quick_copy_backward(void * dst, const void * src, size_t size) _NE
    {
        switch (size)
        {
            #define __Case(_size)                                                       \
                case _size:                                                             \
                    al::quick_copy<_size, true>(dst, src);                              \
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


    void quick_copy(void * dst, const void * src, size_t size) _NE
    {
        if (dst == src || size == 0)
            return;

        if (dst > src && dst <= (const byte_t *)src + size) // Copy backward
        {
            byte_t * dst0 = (byte_t *)dst + size;
            const byte_t * src0 = (const byte_t *)src + size;

            size_t offset = (arch_uint_t)src0 % sizeof(uint64_t);
            if (offset > 0)
            {
                src0 -= offset;
                dst0 -= offset;
                size -= offset;

                __quick_copy_backward(dst0, src0, offset);
            }

            uint64_t * dst1 = (uint64_t *)dst0;
            const uint64_t * src1 = (const uint64_t *)src0;

            while (size >= sizeof(uint64_t))
            {
                *--dst1 = *--src1;
                size -= sizeof(uint64_t);
            }

            __quick_copy_backward(dst, src, size);
        }
        else
        {
            byte_t * dst0 = (byte_t *)dst;
            const byte_t * src0 = (const byte_t *)src;

            size_t offset = (arch_uint_t)src0 % sizeof(uint64_t);
            if (offset > 0)
            {
                __quick_copy(dst0, src0, offset);

                src0 += offset;
                dst0 += offset;
                size -= offset;
            }

            uint64_t * dst1 = (uint64_t *)dst0;
            const uint64_t * src1 = (const uint64_t *)src0;

            while (size >= sizeof(uint64_t))
            {
                *dst1++ = *src1++;
                size -= sizeof(uint64_t);
            }

            __quick_copy(dst1, src1, size);
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
            #define __Case(_size)                                                       \
                case _size:                                                             \
                    al::quick_zero<_size>(p1);                                          \
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


}  // X_ROOT_NS::algorithm
