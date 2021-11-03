
#ifndef __OS_H__
#define __OS_H__

#include <common.h>

namespace X_ROOT_NS { namespace os {

    ////////// ////////// ////////// ////////// //////////

    // Allocates memory of specified size.
    void * malloc(size_t size);

    // Allocates memory of specified element size and count.
    void * calloc(size_t n, size_t size);

    // Reallocates memory of specified size.
    void * realloc(void * p, size_t size);

    // Frees memory.
    void free(void * p);

    // Exit program with specified code.
    void exit(int code);

    ////////// ////////// ////////// ////////// //////////

} }  // namespace X_ROOT_NS::os


#endif // __OS_H__

