
#ifndef __ARCH_H__
#define __ARCH_H__

#include <common.h>

namespace X_ROOT_NS::arch {

    ////////// ////////// ////////// ////////// //////////

    // Prints current execute stack.
    void print_call_stack(int trim = 1);

    // Returns a string of current stack.
    string_t call_stack(int trim = 1);

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::arch


#endif  // __ARCH_H__

