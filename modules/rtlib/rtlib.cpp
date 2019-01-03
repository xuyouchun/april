#include <rtlib.h>

namespace X_ROOT_NS { namespace modules { namespace rtlib {

    ////////// ////////// ////////// ////////// //////////

    class rtlib_manager_t : public object_t
    {
    public:
        rtlib_manager_t()
        {
            __append_libfuncs();
        }

        libfunc_t get_func(const string_t & name)
        {
            auto it = __func_map.find(name);
            if(it != __func_map.end())
                return it->second;

            return nullptr;
        }
    
    private:
        std::map<string_t, libfunc_t> __func_map;

        void __append_libfunc(const string_t & name, libfunc_t libfunc)
        {
            __func_map[name] = libfunc;
        }

        void __append_libfuncs()
        {
            __append_libfunc(_T("Write"), write);
            __append_libfunc(_T("TestAdd"), test_add);
            __append_libfunc(_T("WriteInt32"), write_int32);
            __append_libfunc(_T("WriteBoolean"), write_boolean);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    libfunc_t get_libfunc(const string_t & name)
    {
        static rtlib_manager_t rtlib_manager;
        return rtlib_manager.get_func(name);
    }

    ////////// ////////// ////////// ////////// //////////

} } }


