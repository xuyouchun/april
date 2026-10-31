// Minimal shim of the april runtime headers, for standalone gc tests.
#pragma once
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <cstdio>

#define X_ROOT_NS           april
#define X_ALWAYS_INLINE     inline
#define X_INLINE            inline
#define X_INTERFACE         struct
#define X_TO_STRING_IMPL(s)
#define _T(s)               s
#define _A(x)               assert(x)
#define _E(code)            std::runtime_error(#code)
#define _ED(code, ...)      std::runtime_error(#code)

namespace april {

    typedef unsigned char byte_t;

    template<typename T> constexpr T _alignf(T v, size_t a)
    { return (T)(((size_t)v + a - 1) & ~(a - 1)); }

    inline size_t unit_align(size_t offset, size_t size)
    {
        switch (size)
        {
            case 1: return offset;
            case 2: return _alignf(offset, 2);
            case 3: case 4: return _alignf(offset, 4);
            default: return _alignf(offset, 8);
        }
    }

    namespace al { using std::max; }

    class object_t { public: virtual ~object_t() { } };

    class memory_t
    {
    public:
        template<typename obj_t, typename ... args_t>
        static obj_t * new_obj(memory_t *, args_t && ... args)
        { return new obj_t(std::forward<args_t>(args) ...); }
    };

    enum class common_error_code_t { no_memory };

    namespace modules::core {
        typedef uint64_t rt_stack_unit_t;
    }

    namespace modules::rt {

        using namespace core;

        typedef int32_t  array_length_t;
        typedef uint8_t  dimension_t;

        struct rt_ref_t
        {
            rt_ref_t() = default;
            rt_ref_t(void * ptr) : ptr(ptr) { }
            operator void * () const { return ptr; }
            void * ptr = nullptr;
        };

        class rt_type_t
        {
        public:
            rt_type_t(size_t size) : __size(size) { }
            size_t get_size() { return __size; }
        private:
            size_t __size;
        };

        class rt_array_type_t : public rt_type_t
        {
        public:
            rt_array_type_t(dimension_t dim, rt_type_t * element)
                : rt_type_t(sizeof(void *)), dimension(dim), element_type(element) { }
            dimension_t dimension;
            rt_type_t * element_type;
        };

        struct gc_scan_info_t
        {
            enum class kind_t : uint8_t { conservative, none, ranges };
            kind_t kind = kind_t::conservative;
            std::vector<std::pair<uint32_t, uint32_t>> ranges;
        };

        typedef std::function<bool (rt_type_t * type, gc_scan_info_t & info)>
                                                                    gc_type_resolver_t;

        X_INTERFACE gc_root_source_t
        {
            virtual void each_gc_root_ranges(
                    const std::function<void (void **, void **)> & f) = 0;
            virtual void each_gc_roots(const std::function<void (rt_ref_t)> & f) = 0;
        };

        X_INTERFACE rt_heap_t
        {
            virtual rt_ref_t new_obj(rt_type_t * type) = 0;
            virtual rt_ref_t new_array(rt_array_type_t * type,
                                       const array_length_t * lengths) = 0;
            virtual void * alloc(size_t size, void * realloc = nullptr) = 0;
            virtual void free(void * ptr) = 0;
            virtual void add_root_source(gc_root_source_t *) { }
            virtual void remove_root_source(gc_root_source_t *) { }
            virtual void set_gc_type_resolver(const gc_type_resolver_t &) { }
            virtual size_t collect() { return 0; }
        };
    }

    namespace modules::mm {

        using namespace core;
        using namespace rt;

        enum class memory_error_code_t { array_dimension_error };

        struct gc_params_t
        {
            size_t  initial_threshold   = 16 * 1024 * 1024;
            double  growth_factor       = 2.0;
            bool    stress              = false;
            bool    scan_native_stack   = true;
            bool    verify              = false;
            bool    trace               = false;
        };

        rt_heap_t * create_gc_heap(memory_t * memory,
                                   const gc_params_t & params = gc_params_t());

        X_INLINE array_length_t * get_array_lengths(void * arr_obj)
        {
            return (array_length_t *)((rt_type_t **)arr_obj - 1) - 2;
        }

        X_INLINE rt_type_t * get_object_type(void * obj)
        {
            return *((rt_type_t **)obj - 1);
        }

        X_INLINE array_length_t get_array_length(void * arr_obj)
        {
            return *((array_length_t *)((rt_type_t **)arr_obj - 1) - 1);
        }
    }
}
