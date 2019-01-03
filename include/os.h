
#ifndef __OS_H__
#define __OS_H__

#include <common.h>

namespace X_ROOT_NS { namespace os {

    void * malloc(size_t size);

    void * calloc(size_t n, size_t size);

    void free(void * p);

    void exit(int code);

} }  // namespace X_ROOT_NS::os


#endif // __OS_H__

