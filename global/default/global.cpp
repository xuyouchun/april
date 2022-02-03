
#include <global.h>

#include <iostream>

////////// ////////// ////////// ////////// //////////

namespace X_ROOT_NS {

#if ! CONFIG_NULLPTR_SUPPORTED

const __nullptr_t nullptr;

#endif

}  // namespace X_ROOT_NS


////////// ////////// ////////// ////////// //////////


namespace X_ROOT_NS::compile {


void test() {}


}   // namespace X_ROOT_NS::compile



