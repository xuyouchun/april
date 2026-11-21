
#ifndef __NATIVE_ABI_H__
#define __NATIVE_ABI_H__

// The ABI between the generated code of the native back end and the native
// runtime support (native_rt.cpp). The generated .ll declares and calls these
// symbols; they are plain C functions (no name mangling).

#include <cstdint>

// Symbol names, used by the code generator.
#define APRIL_NATIVE_SYM_INIT           "april_native_init"
#define APRIL_NATIVE_SYM_SHUTDOWN       "april_native_shutdown"
#define APRIL_NATIVE_SYM_FIND_LIBFUNC   "april_native_find_libfunc"
#define APRIL_NATIVE_SYM_INVOKE         "april_native_invoke"
#define APRIL_NATIVE_SYM_NEW_OBJECT     "april_native_new_object"
#define APRIL_NATIVE_SYM_FIND_IFACE     "april_native_find_interface_function"
#define APRIL_NATIVE_SYM_IS_INSTANCE    "april_native_is_instance"
#define APRIL_NATIVE_SYM_REFLECT_CHECK  "april_native_reflect_check"
#define APRIL_NATIVE_SYM_REFLECT_GET    "april_native_reflect_get"
#define APRIL_NATIVE_SYM_REFLECT_SET    "april_native_reflect_set"
#define APRIL_NATIVE_SYM_ENUM_NUMBER    "april_native_enum_number"
#define APRIL_NATIVE_SYM_THREAD_STATIC  "april_native_thread_static"
#define APRIL_NATIVE_SYM_THREAD_START   "april_native_thread_start"
#define APRIL_NATIVE_SYM_YIELD_FLAG     "april_native_yield_flag"
#define APRIL_NATIVE_SYM_SAFEPOINT      "april_native_safepoint"
#define APRIL_NATIVE_SYM_SET_EXCEPTION  "april_native_set_exception_slot"
#define APRIL_NATIVE_SYM_ENUM_TO_STRING "april_native_enum_to_string"
#define APRIL_NATIVE_SYM_ENUM_PARSE     "april_native_enum_parse"
#define APRIL_NATIVE_SYM_REFLECT_CHECK_INVOKE "april_native_reflect_check_invoke"
#define APRIL_NATIVE_SYM_REFLECT_INVOKE "april_native_reflect_invoke"
#define APRIL_NATIVE_SYM_ATTR_COUNT     "april_native_reflect_attribute_count"
#define APRIL_NATIVE_SYM_ATTR_CTOR      "april_native_reflect_attribute_constructor"
#define APRIL_NATIVE_SYM_ATTR_ARG_COUNT "april_native_reflect_attribute_argument_count"
#define APRIL_NATIVE_SYM_ATTR_ARG       "april_native_reflect_attribute_argument"
#define APRIL_NATIVE_SYM_ATTR_NAMED_COUNT "april_native_reflect_attribute_named_count"
#define APRIL_NATIVE_SYM_ATTR_NAMED_NAME  "april_native_reflect_attribute_named_name"
#define APRIL_NATIVE_SYM_ATTR_NAMED_VALUE "april_native_reflect_attribute_named_value"
#define APRIL_NATIVE_SYM_CAST           "april_native_cast"
#define APRIL_NATIVE_SYM_UNHANDLED      "april_native_unhandled_exception"
#define APRIL_NATIVE_SYM_ADD_ROOT       "april_native_add_root"
#define APRIL_NATIVE_SYM_NEW_ARRAY      "april_native_new_array"
#define APRIL_NATIVE_SYM_ARRAY_ELEMENT  "april_native_array_element"
#define APRIL_NATIVE_SYM_ARRAY_TRY      "april_native_array_try_element"
#define APRIL_NATIVE_SYM_ARRAY_LINEAR   "april_native_array_linear_element"
#define APRIL_NATIVE_SYM_ARRAY_LENGTH   "april_native_array_length"
#define APRIL_NATIVE_SYM_ARRAY_RANK     "april_native_array_rank"
#define APRIL_NATIVE_SYM_ARRAY_LENGTH_OF "april_native_array_length_of_dimension"
#define APRIL_NATIVE_SYM_ADD_RESOURCE   "april_native_add_resource"

extern "C" {

    // Initializes the native runtime; called by the generated `main` before the
    // entry point of the program. The garbage collector scans the stack up to
    // `stack_base`: an address in the frame of `main`.
    void april_native_init(void * stack_base);

    // Registers a global of the program that holds an object: it is a root of the
    // garbage collector.
    void april_native_add_root(void ** slot);

    // Registers a resource of the program (the bytes that are embedded in the assemblies, Resource.GetBytes):
    // `name` is UTF-8, NUL-terminated, `data` has `size` bytes. They are constants of the program, not copied.
    void april_native_add_resource(const char * name, const void * data, int64_t size);

    // Releases the native runtime; called after the entry point returned.
    void april_native_shutdown();

    // Finds the runtime library function (rtlib) with the specified name, e.g.
    // "Console_WriteStringLine"; returns nullptr if there is none.
    void * april_native_find_libfunc(const char * name);

    // Calls a runtime library function. `args` is the argument block, with the
    // layout of the interpreter's stack: consecutive 8 byte units, each argument
    // takes ceil(size / 8) units, in declaration order. The return value, if any,
    // is written to the first unit of the block.
    void april_native_invoke(void * libfunc, uint64_t * args);

    // Allocates a zeroed object of `size` bytes; returns its address. The word before
    // the object holds `type`, the address of the type descriptor (see below).
    // Objects and arrays are freed by the garbage collector (native_gc.cpp) when nothing
    // reaches them: from the stack, the registers, the roots or other objects.
    void * april_native_new_object(uint64_t size, void * type);

    // The type descriptor of a class, generated into the program:
    //
    //      +0              the interface table (see april_native_interface_entry_t), or null
    //      +8              the number of entries of the interface table
    //      +16             the cast table: the keys (addresses of globals of the program)
    //                      of the types the class is derived from or implements, or null
    //      +24             the number of keys of the cast table
    //      +32             the virtual function table: one function address per virtual
    //                      offset (null where the class has no function)
    //      +32 + 8 * vcount    the name of the type, then its full name (strings), then the
    //                          table of reflection of the type (rt_reflect_type_t of rt.h), or
    //                          null when the program does not use the reflection
    //
    // Finds the function an interface method of the object's class: `key` identifies the
    // interface (the address of a global of the program), `offset` is the index of the
    // method in the interface. Returns null if the class does not implement it.
    void * april_native_find_interface_function(void * type, void * key, int64_t offset);

    // Returns 1 if the object is an instance of the type (null is not), else 0.
    int8_t april_native_is_instance(void * object, void * key);

    // Returns the object that holds the thread static fields of a class for the running thread (made on the first call, the
    // size and the descriptor are those of the object of the static data of the class); a root of the collector.
    void * april_native_thread_static(int64_t size, void * descriptor);

    // Tells the runtime where the pending exception of the program is (the variable that every call is followed by a check
    // of): the threads that run one at a time keep it apart. Called by `main`.
    void april_native_set_exception_slot(void ** slot);

    // Starts a thread that runs `entry(thread)` (a function of the program, System.Threading.Thread.__Entry), returns its id.
    int64_t april_native_thread_start(void * entry, void * thread, int8_t background);

    // The flag that tells that another thread asks the one that runs to give its turn: the jumps that go back read it, and
    // call april_native_safepoint when it is not 0.
    volatile uint8_t * april_native_yield_flag();

    // Gives the turn to the threads that wait for it.
    void april_native_safepoint();

    // Returns the number of a box of an enum, of its underlying type; `table` is the table of reflection of its type.
    int64_t april_native_enum_number(const void * table, void * object);

    // Returns the text of a box of an enum (the name of its item, or its number), a string.
    const void * april_native_enum_to_string(const void * table, void * object);

    // Returns the box of the value of a text, or null if the text is not a value of the enum: `type` is the descriptor of
    // the enum, `table` is its table of reflection.
    void * april_native_enum_parse(void * type, const void * table, const void * text, int8_t ignore_case);

    // The table of a member of a type, for the reflection (rt_reflect_member_t of rt.h): the
    // fields have the offset of the value, the size, the kind (1 a reference, 2 a value that is
    // boxed, 3 a string), the key of the type that has the field (the address of a global that
    // identifies it for april_native_is_instance), or, for a static field, the address of the
    // global that holds the object of the static data, and the key of the type of the field.
    typedef struct april_native_member_t
    {
        const void *    name;
        void *          declaring;
        void *          type;
        int64_t         flags;
        void *          accessor_a;
        void *          accessor_b;
        int64_t         param_count;
        void *          params;
        int64_t         gparam_count;
        void *          gparams;
        void *          accessor_c;
        int64_t         offset;
        void *          key_or_static;
        int64_t         size;
        int64_t         value_kind;
        void *          type_key;
        void *          invoker;
        void *          attributes;
    } april_native_member_t;

    // The attributes of a member (the field `attributes` of its table, or of the table of a
    // type, +112): null if it has none. Each attribute is the constructor (the table of the
    // method), and the values of its arguments, one for each parameter of the constructor: the
    // bits of a number (in the type of the parameter), or the string.
    //
    // The kind says what the value is when it is not by the parameter (an Object parameter gets the
    // number or the string that was written, and a Type parameter the type): 0 by the parameter,
    // 2 a number (the box is `size` bytes of the class `type`), 3 a string (a box of `type`),
    // 4 a Type (`type` is it), 5 an array of numbers or strings (`bits` is the count of the elements,
    // `string` is the block of them, a unit of 8 bytes for each, and `size` is the size of one).
    typedef struct april_native_attribute_argument_t
    {
        uint64_t        bits;
        const void *    string;
        int64_t         kind;
        int64_t         size;
        void *          type;
    } april_native_attribute_argument_t;

    // A named argument ([Author("ann", Level = 3)]): the name, the table of the field or of the
    // property, the value.
    typedef struct april_native_attribute_named_t
    {
        const void *    name;
        void *          member;
        uint64_t        bits;
        const void *    string;
        int64_t         kind;
        int64_t         size;
        void *          type;
    } april_native_attribute_named_t;

    typedef struct april_native_attribute_t
    {
        void *          constructor;
        april_native_attribute_argument_t * arguments;
        int64_t         named_count;
        april_native_attribute_named_t * named;
    } april_native_attribute_t;

    typedef struct april_native_attributes_t
    {
        int64_t                     count;
        april_native_attribute_t *  items;
    } april_native_attributes_t;

    // The count of the attributes, the constructor of one, the count of its arguments, and one of
    // them boxed (an object of the type of the parameter, or null).
    int32_t april_native_reflect_attribute_count(const april_native_attributes_t * attributes);
    void * april_native_reflect_attribute_constructor(const april_native_attributes_t * attributes,
                                                      int32_t index);
    int32_t april_native_reflect_attribute_argument_count(
                                const april_native_attributes_t * attributes, int32_t index);
    void * april_native_reflect_attribute_argument(const april_native_attributes_t * attributes,
                                                   int32_t index, int32_t position);

    // The named arguments of an attribute: their count, the name of one, its value boxed.
    int32_t april_native_reflect_attribute_named_count(
                                const april_native_attributes_t * attributes, int32_t index);
    const void * april_native_reflect_attribute_named_name(
                                const april_native_attributes_t * attributes, int32_t index,
                                int32_t position);
    void * april_native_reflect_attribute_named_value(const april_native_attributes_t * attributes,
                                                      int32_t index, int32_t position);

    // Checks the access to a field: 0 if the field of the object can be read (mode 0) or written
    // with the value (mode 1), 1 if the object is null, 2 if the object or the value are not of
    // the types that are needed.
    int32_t april_native_reflect_check(const april_native_member_t * member, void * object,
                                       void * value, int32_t mode);

    // Reads the value of a field, boxed if it is not a reference (the access was checked).
    void * april_native_reflect_get(const april_native_member_t * member, void * object);

    // Writes the value of a field, that is the box of a value if it is not a reference.
    void april_native_reflect_set(const april_native_member_t * member, void * object, void * value);

    // Method.Invoke. A method has, in its table (april_native_member_t): `key_or_static`, the
    // function; `offset`, its virtual offset or -1; `type_key`, the key of the type that has it;
    // `value_kind` and `size`, the result (0 for none); `invoker`, the function that calls it:
    //
    //      void invoker(void * function, uint64_t * values, uint64_t * result)
    //
    // `values` has one 8 byte unit for each argument, the object first for a method that is not
    // static; a number is in the low bytes of its unit. The program makes an invoker for each
    // shape of signature that its methods have. The functions are null (key_or_static is) for a
    // method that has no function: that of a type that has no objects, an abstract one, one that
    // takes or returns a struct.
    //
    // Checks the call: 0 if it can be done, 1 if the object is null (and the method is not
    // static), 2 if the object or an argument are not of the types that are needed, or the method
    // cannot be called. `args` is the array of the arguments (the objects and the boxes).
    int32_t april_native_reflect_check_invoke(const april_native_member_t * member, void * object,
                                              void ** args);

    // Calls the method (it was checked), returns its result: a reference is the object, a number,
    // an enum, a bool, a char and a string are boxed, null for a method of void.
    void * april_native_reflect_invoke(const april_native_member_t * member, void * object,
                                       void ** args);

    // Returns the object if it is null or an instance of the type; exits the program
    // otherwise (there are no exceptions in native programs yet).
    void * april_native_cast(void * object, void * key);

    // Arrays. An array is the address of its first element; before it:
    //
    //      -8              the type descriptor (null: arrays have none yet)
    //      -12             int32: the dimension
    //      -16             int32: the total length
    //      -16 - 4 * dimension     the length of each dimension (int32, first one lowest)
    //
    // Allocates a zeroed array; `lengths` has one int32 per dimension. `has_refs` is 0 for
    // arrays of numbers: the collector does not scan them.
    void * april_native_new_array(int64_t element_size, int32_t dimension,
                                  const int32_t * lengths, void * type, int32_t has_refs);

    // Returns the address of an element, `indices` has one int32 per dimension; exits the
    // program if an index is out of range (there are no exceptions yet).
    void * april_native_array_element(void * array, const int32_t * indices,
                                      int32_t dimension, int64_t element_size);

    // Like april_native_array_element, but returns null if an index is out of range (and
    // for a null array): the generated code raises the exceptions.
    void * april_native_array_try_element(void * array, const int32_t * indices,
                                          int32_t dimension, int64_t element_size);

    // Returns the address of an element by its index among all the elements (row major).
    void * april_native_array_linear_element(void * array, int32_t index, int64_t element_size);

    int32_t april_native_array_length(void * array);
    int32_t april_native_array_rank(void * array);
    int32_t april_native_array_length_of_dimension(void * array, int32_t dimension);

    // Exceptions: a thrown exception is the global `april.exception` of the program (the
    // object), which every call checks on return; null when none is pending.
    // Called by `main` when the entry point returned with an exception pending: reports
    // it and exits the program.
    void april_native_unhandled_exception(void * exception);

    struct april_native_interface_entry_t
    {
        void *      key;
        int64_t     offset;
        void *      function;
    };

}

#endif  // __NATIVE_ABI_H__
