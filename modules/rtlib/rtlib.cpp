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
            if (it != __func_map.end())
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
            __append_libfunc(_T("Console_WriteString"),     console_write_string);
            __append_libfunc(_T("Console_WriteChar"),       console_write_char);
            __append_libfunc(_T("Console_WriteInt8"),       console_write_int8);
            __append_libfunc(_T("Console_WriteUInt8"),      console_write_uint8);
            __append_libfunc(_T("Console_WriteInt16"),      console_write_int16);
            __append_libfunc(_T("Console_WriteUInt16"),     console_write_uint16);
            __append_libfunc(_T("Console_WriteInt32"),      console_write_int32);
            __append_libfunc(_T("Console_WriteUInt32"),     console_write_uint32);
            __append_libfunc(_T("Console_WriteInt64"),      console_write_int64);
            __append_libfunc(_T("Console_WriteUInt64"),     console_write_uint64);
            __append_libfunc(_T("Console_WriteSingle"),     console_write_float);
            __append_libfunc(_T("Console_WriteDouble"),     console_write_double);

            __append_libfunc(_T("Console_WriteStringLine"),     console_write_string_line);
            __append_libfunc(_T("Console_WriteCharLine"),       console_write_char_line);
            __append_libfunc(_T("Console_WriteInt8Line"),       console_write_int8_line);
            __append_libfunc(_T("Console_WriteUInt8Line"),      console_write_uint8_line);
            __append_libfunc(_T("Console_WriteInt16Line"),      console_write_int16_line);
            __append_libfunc(_T("Console_WriteUInt16Line"),     console_write_uint16_line);
            __append_libfunc(_T("Console_WriteInt32Line"),      console_write_int32_line);
            __append_libfunc(_T("Console_WriteUInt32Line"),     console_write_uint32_line);
            __append_libfunc(_T("Console_WriteInt64Line"),      console_write_int64_line);
            __append_libfunc(_T("Console_WriteUInt64Line"),     console_write_uint64_line);
            __append_libfunc(_T("Console_WriteSingleLine"),     console_write_float_line);
            __append_libfunc(_T("Console_WriteDoubleLine"),     console_write_double_line);

            // Array functions.
            __append_libfunc(_T("Array_GetRank"),           array_get_rank);
            __append_libfunc(_T("Array_GetLength"),         array_get_length);
            __append_libfunc(_T("Array_GetLengthOfDimension"), array_get_length_of_dimension);

            // Tuple functions.
            __append_libfunc(_T("Tuple_SetRange"),          tuple_set_range);
            __append_libfunc(_T("Tuple_Get"),               tuple_get);
            __append_libfunc(_T("Tuple_GetCount"),          tuple_get_count);

            // Reflection functions.
            __append_libfunc(_T("Reflection_GetType"),      reflection_get_type);
            __append_libfunc(_T("Reflection_GetTypeName"),  reflection_get_type_name);

            // Math functions.
            __append_libfunc(_T("Math_Sqrt"),               math_sqrt);
            __append_libfunc(_T("Math_Pow"),                math_pow);

            __append_libfunc(_T("Math_Sin"),                math_sin);
            __append_libfunc(_T("Math_Cos"),                math_cos);
            __append_libfunc(_T("Math_Tan"),                math_tan);

            __append_libfunc(_T("Math_Sinh"),               math_sinh);
            __append_libfunc(_T("Math_Cosh"),               math_cosh);
            __append_libfunc(_T("Math_Tanh"),               math_tanh);

            __append_libfunc(_T("Math_Asin"),               math_asin);
            __append_libfunc(_T("Math_Acos"),               math_acos);
            __append_libfunc(_T("Math_Atan"),               math_atan);

            __append_libfunc(_T("Math_Asinh"),              math_asinh);
            __append_libfunc(_T("Math_Acosh"),              math_acosh);
            __append_libfunc(_T("Math_Atanh"),              math_atanh);

            __append_libfunc(_T("Math_Log"),                math_log);
            __append_libfunc(_T("Math_Log10"),              math_log10);
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


