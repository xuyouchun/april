#include <rtlib.h>

namespace X_ROOT_NS { namespace modules { namespace rtlib {

    ////////// ////////// ////////// ////////// //////////

    // Runtime lib manager.
    class rtlib_manager_t : public object_t
    {
    public:

        // Constructor.
        rtlib_manager_t()
        {
            __append_libfuncs();
        }

        // Gets function of specified name.
        libfunc_t get_func(const string_t & name)
        {
            auto it = __func_map.find(name);
            if(it != __func_map.end())
                return it->second;

            return nullptr;
        }
    
    private:
        std::map<string_t, libfunc_t> __func_map;

        // Appends lib function.
        void __append_libfunc(const string_t & name, libfunc_t libfunc)
        {
            __func_map[name] = libfunc;
        }

        // Appends lib functions.
        void __append_libfuncs()
        {
            // System fynctions
            __append_libfunc(_T("Console_WriteString"),		console_write_string);
            __append_libfunc(_T("Console_WriteChar"),		console_write_char);
            __append_libfunc(_T("Console_WriteInt8"),		console_write_int8);
            __append_libfunc(_T("Console_WriteUInt8"),		console_write_uint8);
            __append_libfunc(_T("Console_WriteInt16"),		console_write_int16);
            __append_libfunc(_T("Console_WriteUInt16"),		console_write_uint16);
            __append_libfunc(_T("Console_WriteInt32"),		console_write_int32);
            __append_libfunc(_T("Console_WriteUInt32"),		console_write_uint32);
            __append_libfunc(_T("Console_WriteInt64"),		console_write_int64);
            __append_libfunc(_T("Console_WriteUInt64"),		console_write_uint64);
            __append_libfunc(_T("Console_WriteSingle"),		console_write_float);
            __append_libfunc(_T("Console_WriteDouble"),		console_write_double);

            // Array functions.
            __append_libfunc(_T("Array_GetRank"), array_get_rank);
            __append_libfunc(_T("Array_GetLength"), array_get_length);
            __append_libfunc(_T("Array_GetLengthOfDimension"), array_get_length_of_dimension);

			// Tuple functions.
			__append_libfunc(_T("Tuple_SetRange"), tuple_set_range);
			__append_libfunc(_T("Tuple_Get"), tuple_get);
			__append_libfunc(_T("Tuple_GetCount"), tuple_get_count);
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Gets lib function.
    libfunc_t get_libfunc(const string_t & name)
    {
        static rtlib_manager_t rtlib_manager;
        return rtlib_manager.get_func(name);
    }

    ////////// ////////// ////////// ////////// //////////

} } }


