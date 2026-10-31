
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

extern "C" {

    // Initializes the native runtime; called by the generated `main` before the
    // entry point of the program. The garbage collector scans the stack up to
    // `stack_base`: an address in the frame of `main`.
    void april_native_init(void * stack_base);

    // Registers a global of the program that holds an object: it is a root of the
    // garbage collector.
    void april_native_add_root(void ** slot);

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
    //
    // Finds the function an interface method of the object's class: `key` identifies the
    // interface (the address of a global of the program), `offset` is the index of the
    // method in the interface. Returns null if the class does not implement it.
    void * april_native_find_interface_function(void * type, void * key, int64_t offset);

    // Returns 1 if the object is an instance of the type (null is not), else 0.
    int8_t april_native_is_instance(void * object, void * key);

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
