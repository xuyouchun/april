
#ifndef __GLOBAL_BASE_H__
#define __GLOBAL_BASE_H__

#include <config.h>
#define X_ROOT_NS     CONFIG_BASE_NAMESPACE


#ifdef NULL
#undef NULL
#endif


namespace X_ROOT_NS {

#if ! CONFIG_NULLPTR_SUPPORTED

    class __nullptr_t
    {
    public:
        __nullptr_t()
        {

        }

        template<typename T> operator T * () const
        {
            return (T *)0;
        }

        template<class C, class T> operator T C::* () const
        {
            return (T C::*)0;
        }

    private:
        void operator & () const;
    };


    extern const __nullptr_t nullptr;

    #define NULL ::X_ROOT_NS::nullptr

#else

    #define NULL nullptr

#endif  // CONFIG_NULLPTR != true


}  // namespace X_ROOT_NS



#endif  //__GLOBAL_BASE_H__


