
#include <common.h>
#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

namespace X_ROOT_NS { namespace os {

    void * malloc(size_t size)
    {
        return ::malloc(size);
    }

    void * calloc(size_t n, size_t size)
    {
        return ::calloc(n, size);
    }

    void free(void * p)
    {
        if(p)
        {
            ::free(p);
        }
    }

    void exit(int code)
    {
        ::exit(code);
    }


} }  // namespace X_ROOT_NS::os

