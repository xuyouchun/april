
#ifndef __JC_H__
#define __JC_H__

namespace X_ROOT_NS { namespace modules {

    ////////// ////////// ////////// ////////// //////////

    #define JC_ASSEMBLY_FLAG    _T("955825bf-d13a-4d1c-90d5-478e62ceaab6")
    #define JC_ASSEMBLY_EXT     _T("jcd")

    #define JC_MAX_HEAP_SIZE    (64 * 1024 * 1024)

    #define JC_COMPILE_ATTRIBUTE_NAME _T("__compile_attribute__")

    ////////// ////////// ////////// ////////// //////////

    struct rt_ref_t
    {
        rt_ref_t() = default;
        rt_ref_t(void * ptr) : ptr(ptr) { }

        void * ptr;

        operator byte_t * () const { return (byte_t *)ptr; }
        rt_ref_t operator = (byte_t * ptr) { this->ptr = ptr; return *this; }

        operator void * () const { return ptr; }
        rt_ref_t operator = (void * ptr) { this->ptr = ptr; return *this; }
    };

    template<typename _stream_t>
    _stream_t & operator << (_stream_t & stream, rt_ref_t ref)
    {
        return stream << ref.ptr, stream;
    }

    //-------- ---------- ---------- ---------- ----------

    const size_t rt_ref_size = sizeof(rt_ref_t);

    typedef arch_uint_t rt_stack_unit_t;

    typedef int32_t array_length_t;

    ////////// ////////// ////////// ////////// //////////

} }

#endif  // __JC_H__
