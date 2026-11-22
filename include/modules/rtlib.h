#ifndef __RTLIB_H__
#define __RTLIB_H__

#include <common.h>
#include <algorithm.h>
#include <jc.h>
#include <rt.h>
#include <thread_manager.h>

namespace X_ROOT_NS::modules::rtlib {

    using namespace rt;

    struct rtlib_context_t;

    typedef void (*libfunc_t)(rtlib_context_t & ctx);

    // Gets libfunc of specified name.
    libfunc_t get_libfunc(const string_t & name);

    namespace
    {
        typedef rt_stack_unit_t __unit_t;
    }

    ////////// ////////// ////////// ////////// //////////

    // Runtime argument.
    template<typename t> struct rt_arg_t
    {
        // Constructor.
        rt_arg_t() = default;
        rt_arg_t(t value) : __value(value) { }

        static const int unit_size = _alignf(sizeof(t), sizeof(__unit_t)) / sizeof(__unit_t);

        union
        {
            __unit_t __units[unit_size];
            t        __value;
        };

        // Returns the raw value.
        operator t() { return (t)__value; }
        t operator *() { return (t)__value; }
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime times.

    typedef rt_arg_t<const rt_char_t *> rstring_t;

    // Creates a string of the runtime from a text.
    const rt_char_t * new_rt_string(const std::wstring & text);

    // The names and the values of the items of an enum (enum.cpp), by the table of reflection of its type (the
    // interpreter and the native programs have the same tables).
    const rt_char_t * enum_text_of(const rt_reflect_type_t * table, int64_t number);
    bool enum_parse_text(const rt_reflect_type_t * table, const rt_char_t * text, bool ignore_case, int64_t * out);
    typedef rt_arg_t<rt_char_t>         rchar_t;

    typedef rt_arg_t<int8_t>            rint8_t;
    typedef rt_arg_t<uint8_t>           ruint8_t;

    typedef rt_arg_t<int16_t>           rint16_t;
    typedef rt_arg_t<uint16_t>          ruint16_t;

    typedef rt_arg_t<int32_t>           rint32_t;
    typedef rt_arg_t<uint32_t>          ruint32_t;

    typedef rt_arg_t<int64_t>           rint64_t;
    typedef rt_arg_t<uint64_t>          ruint64_t;

    typedef rt_arg_t<float>             rfloat_t;
    typedef rt_arg_t<double>            rdouble_t;

    typedef rt_arg_t<bool>              rbool_t;
    typedef rt_arg_t<void *>            rptr_t;

    typedef rt_arg_t<rt_ref_t>          robject_t;

    ////////// ////////// ////////// ////////// //////////

    // Return codes of runtime lib functions.
    const int rtlib_ret_null_reference = 1;     // A null reference was used.
    const int rtlib_ret_invalid_cast = 2;       // An object is not of the type that is needed.

    // Runtime lib context.
    struct rtlib_context_t
    {
        // Constructor.
        rtlib_context_t(assembly_analyzer_t & analyzer, rt_stack_unit_t * argument) _NE
                : analyzer(analyzer), argument(argument) { }

        assembly_analyzer_t & analyzer;
        rt_stack_unit_t     * argument;
        // The this of the method that calls the function (the struct that a constructor makes).
        void                * caller_this     = nullptr;

        // The heap that the objects are allocated in.
        rt_heap_t           * heap            = nullptr;

        // The environment of the execution (the analyzer is that of the calling assembly).
        analyzer_env_t      * exec_env        = nullptr;
        int                   ret_code        = 0;
        string_t              ret_message;

        // Arguments.
        template<typename t> t * args() _NE
        {
            return (t *)argument;
        }

        // Returns specified value.
        template<typename t> void return_(t value) _NE
        {
            typedef rt_arg_t<t> rt;
            *(rt *)argument = rt(value);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    #define __DefineFunc(name) void name(rtlib_context_t & ctx)

    // System functions.
    __DefineFunc(console_write_string);
    __DefineFunc(console_write_char);
    __DefineFunc(console_write_int8);
    __DefineFunc(console_write_uint8);
    __DefineFunc(console_write_int16);
    __DefineFunc(console_write_uint16);
    __DefineFunc(console_write_int32);
    __DefineFunc(console_write_uint32);
    __DefineFunc(console_write_int64);
    __DefineFunc(console_write_uint64);
    __DefineFunc(console_write_float);
    __DefineFunc(console_write_double);

    __DefineFunc(console_write_string_line);
    __DefineFunc(console_write_char_line);
    __DefineFunc(console_write_int8_line);
    __DefineFunc(console_write_uint8_line);
    __DefineFunc(console_write_int16_line);
    __DefineFunc(console_write_uint16_line);
    __DefineFunc(console_write_int32_line);
    __DefineFunc(console_write_uint32_line);
    __DefineFunc(console_write_int64_line);
    __DefineFunc(console_write_uint64_line);
    __DefineFunc(console_write_float_line);
    __DefineFunc(console_write_double_line);

    // String functions.
    __DefineFunc(string_concat);
    __DefineFunc(string_equals);
    __DefineFunc(string_compare);
    __DefineFunc(string_get_length);
    __DefineFunc(string_substring);
    __DefineFunc(string_get_hash_code);
    __DefineFunc(int64_to_string);
    __DefineFunc(uint64_to_string);
    __DefineFunc(double_to_string);
    __DefineFunc(float_to_string);
    __DefineFunc(char_to_string);
    __DefineFunc(string_get_char);
    __DefineFunc(string_from_chars);
    __DefineFunc(string_copy_to);
    __DefineFunc(string_index_of);
    __DefineFunc(string_last_index_of);
    __DefineFunc(string_index_of_char);
    __DefineFunc(string_last_index_of_char);
    __DefineFunc(string_trim);
    __DefineFunc(string_is_blank);
    __DefineFunc(utf8_get_byte_count);
    __DefineFunc(utf8_get_bytes);
    __DefineFunc(utf8_get_string);
    __DefineFunc(double_to_fixed);
    __DefineFunc(double_to_scientific);
    __DefineFunc(resource_get_size);
    __DefineFunc(resource_read);
    __DefineFunc(resource_get_ptr);
    __DefineFunc(string_to_upper);
    __DefineFunc(string_to_lower);
    __DefineFunc(char_is_letter);
    __DefineFunc(char_is_digit);
    __DefineFunc(char_is_white_space);
    __DefineFunc(char_to_upper);
    __DefineFunc(char_to_lower);
    __DefineFunc(ptr_from_int64);
    __DefineFunc(ptr_to_int64);
    __DefineFunc(memory_alloc);
    __DefineFunc(memory_free);
    __DefineFunc(time_utc_now_ticks);
    __DefineFunc(time_monotonic_ticks);
    __DefineFunc(memory_copy);
    __DefineFunc(memory_fill);
    __DefineFunc(string_address_of);

    // The allocator of the memory of the strings that programs make (e.g. concatenation).
    // Without one, the memory is never freed.
    typedef void * (* string_allocator_t)(uint64_t bytes);
    void set_string_allocator(string_allocator_t allocator);

    // The resources of a program that has no assemblies (the native ones: the resources are in the program): the
    // function finds the bytes of the resource of a name. Without one, the assemblies that are loaded are searched.
    typedef bool (* resource_finder_t)(const string_t & name, const byte_t ** bytes, uint64_t * length);
    void set_resource_finder(resource_finder_t finder);
    void * allocate_string_memory(size_t bytes);

    // Formats a floating point number like .NET: the shortest text that reads back exactly.
    std::wstring format_floating(double value);
    std::wstring format_floating(float value);

    // Array functions.
    __DefineFunc(array_get_rank);
    __DefineFunc(array_get_length);
    __DefineFunc(array_get_length_of_dimension);

    // Tuple functions.
    __DefineFunc(tuple_set_range);
    __DefineFunc(tuple_create);
    __DefineFunc(tuple_get);
    __DefineFunc(tuple_get_count);

    // Reflection functions.
    __DefineFunc(reflection_get_type);
    __DefineFunc(reflection_get_type_name);
    __DefineFunc(reflection_get_type_full_name);
    __DefineFunc(reflection_type_base);
    __DefineFunc(reflection_type_flags);
    __DefineFunc(reflection_type_assembly);
    __DefineFunc(reflection_member_count);
    __DefineFunc(reflection_nested_type_count);
    __DefineFunc(reflection_nested_type);
    __DefineFunc(reflection_field);
    __DefineFunc(reflection_method);
    __DefineFunc(reflection_property);
    __DefineFunc(reflection_event);
    __DefineFunc(reflection_type_generic_parameter);
    __DefineFunc(reflection_info_name);
    __DefineFunc(reflection_info_declaring_type);
    __DefineFunc(reflection_info_type);
    __DefineFunc(reflection_info_flags);
    __DefineFunc(reflection_property_getter);
    __DefineFunc(reflection_property_setter);
    __DefineFunc(reflection_event_adder);
    __DefineFunc(reflection_event_remover);
    __DefineFunc(reflection_event_raiser);
    __DefineFunc(reflection_get_field_value);
    __DefineFunc(reflection_check_invoke);
    __DefineFunc(reflection_is_member);
    __DefineFunc(reflection_attribute_named_count);
    __DefineFunc(reflection_type_attribute_named_count);
    __DefineFunc(reflection_attribute_named_name);
    __DefineFunc(reflection_type_attribute_named_name);
    __DefineFunc(reflection_attribute_named_value);
    __DefineFunc(reflection_type_attribute_named_value);
    __DefineFunc(reflection_attribute_count);
    __DefineFunc(reflection_type_attribute_count);
    __DefineFunc(reflection_attribute_constructor);
    __DefineFunc(reflection_type_attribute_constructor);
    __DefineFunc(reflection_attribute_argument_count);
    __DefineFunc(reflection_type_attribute_argument_count);
    __DefineFunc(reflection_attribute_argument);
    __DefineFunc(reflection_type_attribute_argument);
    __DefineFunc(reflection_new_object);
    __DefineFunc(reflection_check_field_access);
    __DefineFunc(reflection_set_field_value);
    __DefineFunc(reflection_parameter_count);
    __DefineFunc(reflection_parameter);
    __DefineFunc(reflection_generic_parameter_count);
    __DefineFunc(reflection_generic_parameter);
    __DefineFunc(object_get_hash_code);
    __DefineFunc(enum_get_number);
    __DefineFunc(enum_to_string);
    __DefineFunc(enum_parse);

    // Thread functions: the threads and the locks of the objects.
    __DefineFunc(thread_start);
    __DefineFunc(thread_join);
    __DefineFunc(thread_sleep);
    __DefineFunc(thread_yield);
    __DefineFunc(thread_current_id);
    __DefineFunc(thread_get_current);
    __DefineFunc(thread_is_alive);
    __DefineFunc(thread_set_background);
    __DefineFunc(thread_is_background);
    __DefineFunc(monitor_enter);
    __DefineFunc(monitor_try_enter);
    __DefineFunc(monitor_exit);
    __DefineFunc(monitor_wait);
    __DefineFunc(monitor_pulse);
    __DefineFunc(monitor_is_entered);
    __DefineFunc(environment_processor_count);
    __DefineFunc(thread_is_parallel);
    __DefineFunc(interlocked_add32);
    __DefineFunc(interlocked_add64);
    __DefineFunc(interlocked_exchange32);
    __DefineFunc(interlocked_exchange64);
    __DefineFunc(interlocked_compare_exchange32);
    __DefineFunc(interlocked_compare_exchange64);
    __DefineFunc(object_reference_equals);

    // Math functions.
    __DefineFunc(math_sqrt);
    __DefineFunc(math_pow);

    __DefineFunc(math_sin);
    __DefineFunc(math_cos);
    __DefineFunc(math_tan);

    __DefineFunc(math_sinh);
    __DefineFunc(math_cosh);
    __DefineFunc(math_tanh);

    __DefineFunc(math_asin);
    __DefineFunc(math_acos);
    __DefineFunc(math_atan);

    __DefineFunc(math_asinh);
    __DefineFunc(math_acosh);
    __DefineFunc(math_atanh);

    __DefineFunc(math_log);
    __DefineFunc(math_log10);

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rtlib

#endif  // __RTLIB_H__
