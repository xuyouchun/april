
#include <algorithm.h>

namespace X_ROOT_NS { namespace algorithm {

    ////////// ////////// ////////// ////////// //////////

    // Call the initialize function, implemented in the sub class.
    void initializer_t::__do_init()
    {
        __initialize();
    }

    ////////// ////////// ////////// ////////// //////////

    // Constructor with signal count.
    semaphore_t::semaphore_t(signal_count_t signal_count)
        : __max_signal_count(signal_count), __signal_count(signal_count)
    {
        _L(__mutex);
    }

    // Acquire a signal.
    bool semaphore_t::acquire()
    {
        return acquire(seconds_t(0));
    }

    // Try to acquire a signal.
    bool semaphore_t::try_acquire()
    {
        _L(__mutex);

        return __try_acquire();
    }

    // Try to acquire a signal.
    bool semaphore_t::__try_acquire()
    {
        if(__waiting_count == 0 && __signal_count > 0)
        {
            __signal_count--;
            return true;
        }

        return false;
    }

    // Release a signal.
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
