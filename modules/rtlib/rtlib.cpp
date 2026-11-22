#include <rtlib.h>

namespace X_ROOT_NS::modules::rtlib {

    al::xheap_t __global_heap(_T("rtlib_heap"));

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

        X_TO_STRING_IMPL(_T("rtlib_manager_t"))
    
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
            // System functions
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

            // String functions.
            __append_libfunc(_T("String_Concat"),           string_concat);
            __append_libfunc(_T("String_Equals"),           string_equals);
            __append_libfunc(_T("String_Compare"),          string_compare);
            __append_libfunc(_T("String_GetLength"),        string_get_length);
            __append_libfunc(_T("String_Substring"),        string_substring);
            __append_libfunc(_T("String_GetHashCode"),      string_get_hash_code);
            __append_libfunc(_T("Int64_ToString"),          int64_to_string);
            __append_libfunc(_T("UInt64_ToString"),         uint64_to_string);
            __append_libfunc(_T("Double_ToString"),         double_to_string);
            __append_libfunc(_T("Single_ToString"),         float_to_string);
            __append_libfunc(_T("Char_ToString"),           char_to_string);
            __append_libfunc(_T("String_GetChar"), string_get_char);
            __append_libfunc(_T("String_FromChars"), string_from_chars);
            __append_libfunc(_T("String_CopyTo"), string_copy_to);
            __append_libfunc(_T("String_IndexOf"), string_index_of);
            __append_libfunc(_T("String_LastIndexOf"), string_last_index_of);
            __append_libfunc(_T("String_IndexOfChar"), string_index_of_char);
            __append_libfunc(_T("String_LastIndexOfChar"), string_last_index_of_char);
            __append_libfunc(_T("String_Trim"), string_trim);
            __append_libfunc(_T("String_IsBlank"), string_is_blank);
            __append_libfunc(_T("Utf8_GetByteCount"), utf8_get_byte_count);
            __append_libfunc(_T("Utf8_GetBytes"), utf8_get_bytes);
            __append_libfunc(_T("Utf8_GetString"), utf8_get_string);
            __append_libfunc(_T("Double_ToFixed"), double_to_fixed);
            __append_libfunc(_T("Double_ToScientific"), double_to_scientific);
            __append_libfunc(_T("Resource_GetSize"), resource_get_size);
            __append_libfunc(_T("Resource_Read"), resource_read);
            __append_libfunc(_T("Resource_GetPtr"), resource_get_ptr);
            __append_libfunc(_T("String_ToUpper"), string_to_upper);
            __append_libfunc(_T("String_ToLower"), string_to_lower);
            __append_libfunc(_T("Char_IsLetter"), char_is_letter);
            __append_libfunc(_T("Char_IsDigit"), char_is_digit);
            __append_libfunc(_T("Char_IsWhiteSpace"), char_is_white_space);
            __append_libfunc(_T("Char_ToUpper"), char_to_upper);
            __append_libfunc(_T("Char_ToLower"), char_to_lower);
            __append_libfunc(_T("Ptr_FromInt64"), ptr_from_int64);
            __append_libfunc(_T("Ptr_ToInt64"), ptr_to_int64);
            __append_libfunc(_T("Memory_Alloc"), memory_alloc);
            __append_libfunc(_T("Memory_Free"), memory_free);
            __append_libfunc(_T("Time_UtcNowTicks"), time_utc_now_ticks);
            __append_libfunc(_T("Time_MonotonicTicks"), time_monotonic_ticks);
            __append_libfunc(_T("Memory_Copy"), memory_copy);
            __append_libfunc(_T("Memory_Fill"), memory_fill);
            __append_libfunc(_T("String_AddressOf"), string_address_of);

            // Array functions.
            __append_libfunc(_T("Array_GetRank"),           array_get_rank);
            __append_libfunc(_T("Array_GetLength"),         array_get_length);
            __append_libfunc(_T("Array_GetLengthOfDimension"), array_get_length_of_dimension);

            // Tuple functions.
            __append_libfunc(_T("Tuple_SetRange"),          tuple_set_range);
            __append_libfunc(_T("Tuple_Create"),            tuple_create);
            __append_libfunc(_T("Tuple_Get"),               tuple_get);
            __append_libfunc(_T("Tuple_GetCount"),          tuple_get_count);

            // Reflection functions.
            __append_libfunc(_T("Reflection_GetType"),      reflection_get_type);
            __append_libfunc(_T("Reflection_GetTypeName"),  reflection_get_type_name);
            __append_libfunc(_T("Reflection_GetTypeFullName"), reflection_get_type_full_name);
            __append_libfunc(_T("Reflection_GetTypeBase"), reflection_type_base);
            __append_libfunc(_T("Reflection_GetTypeFlags"), reflection_type_flags);
            __append_libfunc(_T("Reflection_GetTypeAssembly"), reflection_type_assembly);
            __append_libfunc(_T("Reflection_GetMemberCount"), reflection_member_count);
            __append_libfunc(_T("Reflection_GetField"), reflection_field);
            __append_libfunc(_T("Reflection_GetNestedTypeCount"), reflection_nested_type_count);
            __append_libfunc(_T("Reflection_GetNestedType"), reflection_nested_type);
            __append_libfunc(_T("Reflection_GetMethod"), reflection_method);
            __append_libfunc(_T("Reflection_GetProperty"), reflection_property);
            __append_libfunc(_T("Reflection_GetEvent"), reflection_event);
            __append_libfunc(_T("Reflection_GetTypeGenericParameter"), reflection_type_generic_parameter);
            __append_libfunc(_T("Reflection_GetInfoName"), reflection_info_name);
            __append_libfunc(_T("Reflection_GetInfoDeclaringType"), reflection_info_declaring_type);
            __append_libfunc(_T("Reflection_GetInfoType"), reflection_info_type);
            __append_libfunc(_T("Reflection_GetInfoFlags"), reflection_info_flags);
            __append_libfunc(_T("Reflection_GetPropertyGetter"), reflection_property_getter);
            __append_libfunc(_T("Reflection_GetPropertySetter"), reflection_property_setter);
            __append_libfunc(_T("Reflection_GetEventAdder"), reflection_event_adder);
            __append_libfunc(_T("Reflection_GetEventRemover"), reflection_event_remover);
            __append_libfunc(_T("Reflection_GetEventRaiser"), reflection_event_raiser);
            __append_libfunc(_T("Reflection_GetFieldValue"), reflection_get_field_value);
            __append_libfunc(_T("Reflection_CheckInvoke"), reflection_check_invoke);
            __append_libfunc(_T("Reflection_IsMember"), reflection_is_member);
            __append_libfunc(_T("Reflection_GetAttributeNamedCount"), reflection_attribute_named_count);
            __append_libfunc(_T("Reflection_GetTypeAttributeNamedCount"), reflection_type_attribute_named_count);
            __append_libfunc(_T("Reflection_GetAttributeNamedName"), reflection_attribute_named_name);
            __append_libfunc(_T("Reflection_GetTypeAttributeNamedName"), reflection_type_attribute_named_name);
            __append_libfunc(_T("Reflection_GetAttributeNamedValue"), reflection_attribute_named_value);
            __append_libfunc(_T("Reflection_GetTypeAttributeNamedValue"), reflection_type_attribute_named_value);
            __append_libfunc(_T("Reflection_GetAttributeCount"), reflection_attribute_count);
            __append_libfunc(_T("Reflection_GetTypeAttributeCount"), reflection_type_attribute_count);
            __append_libfunc(_T("Reflection_GetAttributeConstructor"), reflection_attribute_constructor);
            __append_libfunc(_T("Reflection_GetTypeAttributeConstructor"), reflection_type_attribute_constructor);
            __append_libfunc(_T("Reflection_GetAttributeArgumentCount"), reflection_attribute_argument_count);
            __append_libfunc(_T("Reflection_GetTypeAttributeArgumentCount"), reflection_type_attribute_argument_count);
            __append_libfunc(_T("Reflection_GetAttributeArgument"), reflection_attribute_argument);
            __append_libfunc(_T("Reflection_GetTypeAttributeArgument"), reflection_type_attribute_argument);
            __append_libfunc(_T("Reflection_NewObject"), reflection_new_object);
            __append_libfunc(_T("Reflection_CheckFieldAccess"), reflection_check_field_access);
            __append_libfunc(_T("Reflection_SetFieldValue"), reflection_set_field_value);
            __append_libfunc(_T("Reflection_GetParameterCount"), reflection_parameter_count);
            __append_libfunc(_T("Reflection_GetParameter"), reflection_parameter);
            __append_libfunc(_T("Reflection_GetGenericParameterCount"), reflection_generic_parameter_count);
            __append_libfunc(_T("Reflection_GetGenericParameter"), reflection_generic_parameter);
            __append_libfunc(_T("Object_GetHashCode"),      object_get_hash_code);
            __append_libfunc(_T("Enum_GetNumber"),          enum_get_number);
            __append_libfunc(_T("Enum_ToString"),           enum_to_string);
            __append_libfunc(_T("Enum_Parse"),              enum_parse);

            // Threads.
            __append_libfunc(_T("Thread_Start"),            thread_start);
            __append_libfunc(_T("Thread_Join"),             thread_join);
            __append_libfunc(_T("Thread_Sleep"),            thread_sleep);
            __append_libfunc(_T("Thread_Yield"),            thread_yield);
            __append_libfunc(_T("Thread_CurrentId"),        thread_current_id);
            __append_libfunc(_T("Thread_GetCurrent"),       thread_get_current);
            __append_libfunc(_T("Thread_IsAlive"),          thread_is_alive);
            __append_libfunc(_T("Thread_SetBackground"),    thread_set_background);
            __append_libfunc(_T("Thread_IsBackground"),     thread_is_background);
            __append_libfunc(_T("Monitor_Enter"),           monitor_enter);
            __append_libfunc(_T("Monitor_TryEnter"),        monitor_try_enter);
            __append_libfunc(_T("Monitor_Exit"),            monitor_exit);
            __append_libfunc(_T("Monitor_Wait"),            monitor_wait);
            __append_libfunc(_T("Monitor_Pulse"),           monitor_pulse);
            __append_libfunc(_T("Monitor_IsEntered"),       monitor_is_entered);
            __append_libfunc(_T("Environment_ProcessorCount"), environment_processor_count);
            __append_libfunc(_T("Object_ReferenceEquals"),  object_reference_equals);

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

}   // namespace X_ROOT_NS::modules::rtlib


