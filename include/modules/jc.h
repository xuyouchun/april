
#ifndef __JC_H__
#define __JC_H__

namespace X_ROOT_NS::modules {

    ////////// ////////// ////////// ////////// //////////

    // Assembly magic code.
    #define JC_ASSEMBLY_FLAG    _T("955825bf-d13a-4d1c-90d5-478e62ceaab6")

    // Assembly default file extention
    #define JC_ASSEMBLY_EXT     _T("apl")

    // Max heap size when running assembly.
    #define JC_MAX_HEAP_SIZE    (64 * 1024 * 1024)

    // Compiler attribute name.
    #define JC_COMPILE_ATTRIBUTE_NAME _T("__955825bf_d13a_4d1c_90d5_478e62ceaab6__")

    // Current assembly version.
    #define JC_VERSION          _T("0.1")

    ////////// ////////// ////////// ////////// //////////

    // a runtime object reference.
    struct rt_ref_t
    {
        rt_ref_t() = default;
        rt_ref_t(void * ptr) : ptr(ptr) { }

        void * ptr;

        operator byte_t * () const { return (byte_t *)ptr; }
        rt_ref_t operator = (byte_t * ptr) { this->ptr = ptr; return *this; }

        operator void * () const { return ptr; }
        rt_ref_t operator = (void * ptr) { this->ptr = ptr; return *this; }

        bool operator == (std::nullptr_t) const _NE { return ptr == nullptr; }
        bool operator != (std::nullptr_t) const _NE { return ptr != nullptr; }
    };

    // Writes a runtime object reference to a stream.
    template<typename _stream_t>
    _stream_t & operator << (_stream_t & stream, rt_ref_t ref)
    {
        return stream << ref.ptr, stream;
    }

    //-------- ---------- ---------- ---------- ----------

    // Object reference size.
    const size_t rt_ref_size = sizeof(rt_ref_t);

    // Stack unit size.
    typedef arch_uint_t rt_stack_unit_t;

    // Array length.
    typedef int32_t array_length_t;

    // Max array dimension.
    const size_t max_array_dimension = 32;

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules

#endif  // __JC_H__
