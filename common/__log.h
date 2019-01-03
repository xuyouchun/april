
#ifndef __COMMON_LOG_H__
#define __COMMON_LOG_H__

namespace X_ROOT_NS {

    enum class log_type_t
    {
        empty       =   0,

        trace       =   1 << 1,

        debug       =   1 << 2,

        info        =   1 << 3,

        warning     =   1 << 4,

        error       =   1 << 5,

        fatal       =   1 << 6,

        all         =   -1,
    };

    //-------- ---------- ---------- ---------- ----------

    namespace __
    {
        void write(log_type_t type, const wchar_t * s);

    }

    //-------- ---------- ---------- ---------- ----------

    void set_log_mask(log_type_t mask);

    log_type_t get_log_mask();

    void switch_log(log_type_t type, bool open);

    bool test_log_mask(log_type_t type);
 
    //-------- ---------- ---------- ---------- ----------

    template<typename format_t, typename ... args_t>
    void logf(log_type_t type, const format_t & format, args_t ... args)
    {
        if(test_log_mask(type))
        {
            string_t s = sprintf(format, args ...);
            __::write(type, s.c_str());
        }
    }

    template<typename ... args_t>
    void log(log_type_t type, args_t ... args)
    {
        if(test_log_mask(type))
        {
            string_t s = sprint(args ...);
            __::write(type, s.c_str());
        }
    }

    //-------- ---------- ---------- ---------- ----------

    #define __X_DEFINE_LOG_FUNC(name, type)                 \
        template<typename ... args_t>                       \
        void name(args_t ... args)                          \
        {                                                   \
            log(log_type_t::type, args ...);                \
        }                                                   \
                                                            \
        template<typename format_t, typename ... args_t>    \
        void name##f(format_t & format, args_t ... args)    \
        {                                                   \
            logf(log_type_t::type, format, args ...);       \
        }

    __X_DEFINE_LOG_FUNC(trace,       trace)
    __X_DEFINE_LOG_FUNC(debug,       debug)
    __X_DEFINE_LOG_FUNC(log_info,    info)
    __X_DEFINE_LOG_FUNC(log_warning, warning)
    __X_DEFINE_LOG_FUNC(log_error,   error)
    __X_DEFINE_LOG_FUNC(log_fatal,   fatal)
}

#endif // __COMMON_LOG_H__

