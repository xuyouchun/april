
#include <common.h>
#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

namespace X_ROOT_NS { namespace os {

    ////////// ////////// ////////// ////////// //////////

    // Allocates memory of specified size.
    void * malloc(size_t size)
    {
        return ::malloc(size);
    }

    // Allocates memory of specified element size and count.
    void * calloc(size_t n, size_t size)
    {
        return ::calloc(n, size);
    }

    // Reallocates memory of specified size.
    void * realloc(void * p, size_t size)
    {
        return ::realloc(p, size);
    }

    // Frees memory.
    void free(void * p)
    {
        if (p)
        {
            ::free(p);
        }
    }

    // Exit program with specified code.
    void exit(int code)
    {
        ::exit(code);
    }

    ////////// ////////// ////////// ////////// //////////

} }  // namespace X_ROOT_NS::os

