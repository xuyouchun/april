
/*
    XuYouChun

    Defines a set of tools of log functions.
*/


#include <common.h>
#include <boost/log/trivial.hpp>

namespace X_ROOT_NS {

    #define __X_BLT(type) BOOST_LOG_TRIVIAL(type)

    //-------- ---------- ---------- ---------- ----------

    namespace __
    {
        // Writes log to the stream that given by log type.
        void write(log_type_t type, const wchar_t * s)
        {
            switch(type)
            {
                case log_type_t::trace:
                    __X_BLT(trace) << s;
                    break;

                case log_type_t::debug:
                    __X_BLT(debug) << s;
                    break;

                case log_type_t::info:
                    __X_BLT(info) << s;
                    break;

                case log_type_t::warning:
                    __X_BLT(warning) << s;
                    break;

                case log_type_t::error:
                    __X_BLT(error) << s;
                    break;

                case log_type_t::fatal:
                    __X_BLT(fatal) << s;
                    break;

                default:
                    break;
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // Log mask to open/close outpt of any log type.
    log_type_t __log_mask = log_type_t::all;

    // Sets log mask to open/close output of specified log type.
    void set_log_mask(log_type_t mask)
    {
        __log_mask = mask;
    }

    // Gets the current log mask.
    log_type_t get_log_mask()
    {
        return __log_mask;
    }

    // Switch output of specified log type.
    void switch_log(log_type_t type, bool open)
    {
        typedef int_type_t<sizeof(type)> t;
        t & mask = *(t *)&__log_mask;
        if(open)
            mask |= (t)type;
        else
            mask &= ~(t)type;
    }

    // Gets the specified if a log type is setted.
    bool test_log_mask(log_type_t type)
    {
        return (int)get_log_mask() & (int)type;
    }

}  // namespace X_ROOT_NS


