
#include <algorithm.h>

namespace X_ROOT_NS { namespace algorithm {

    ////////// ////////// ////////// ////////// //////////

    void initializer_t::__do_init()
    {
        __initialize();
    }

    ////////// ////////// ////////// ////////// //////////

    semaphore_t::semaphore_t(signal_count_t signal_count)
        : __max_signal_count(signal_count), __signal_count(signal_count)
    {
        _L(__mutex);

    }

    bool semaphore_t::acquire()
    {
        return acquire(seconds_t(0));
    }

    bool semaphore_t::try_acquire()
    {
        _L(__mutex);

        return __try_acquire();
    }

    bool semaphore_t::__try_acquire()
    {
        if(__waiting_count == 0 && __signal_count > 0)
        {
            __signal_count--;
            return true;
        }

        return false;
    }

    void semaphore_t::release()
    {
        _L(__mutex);

        if(__signal_count >= __max_signal_count)
            throw _E(common_error_code_t::invalid_operation, _T("signal count overflow"));

        __signal_count++;
        __condition_var.notify_one();
    }

    semaphore_t::~semaphore_t()
    {

    }

    ////////// ////////// ////////// ////////// //////////

} }  // X_ROOT_NS::algorithm
