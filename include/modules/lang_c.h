
#ifndef __LANG_C_H__
#define __LANG_C_H__

#include <compile.h>
#include <modules/lang_cs.h>

namespace X_ROOT_NS::modules::lang_c {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        using namespace compile;
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    class c_lang_service_t;

    // Language c: a statement with the pointers of C (int* p, *p, &x, p->f, (T*)p, stackalloc T[n], sizeof(T)),
    // it is embedded in the code of another language (<%c { ... } %>). Its code is that of cs, with the syntax of
    // the pointers: the nodes that it makes are those of cs (the members of Ptr and Ptr<T>, the calls of NativeMemory).
    class c_lang_t : public lang_base_t
    {
    public:

        // Constructor.
        c_lang_t(const lang_create_args_t & args);

        static const char_t * const __default_name__;

        X_TO_STRING_IMPL(_T("c_lang_t"))
    };

    ////////// ////////// ////////// ////////// //////////

}   // X_ROOT_NS::modules::lang_c

#endif  // __LANG_C_H__
