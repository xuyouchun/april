#include "controller.h"

namespace X_ROOT_NS::modules::compile {

    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    // Method compile controller.
    class __method_compile_controller_t : public method_compile_controller_t
    {
    public:

        // Returns whether specified code need optimized.
        virtual bool optimize(int code) override
        {
            return false;
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Returns a default method compile controller.
    method_compile_controller_t * default_controller()
    {
        static __method_compile_controller_t controller;

        return &controller;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

