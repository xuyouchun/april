
#ifndef __MM_GC_HEAP_H__
#define __MM_GC_HEAP_H__

#include <mm.h>

#include <cstdlib>
#include <functional>
#include <unordered_map>
#include <vector>

namespace X_ROOT_NS::modules::mm {

    namespace
    {
        using namespace core;
        using namespace rt;
    }

    // Garbage collected runtime heap, made on the space of the collector (include/modules/gc_space.h: the memory, the
    // marking and the sweeping). It knows the types of the objects: new_obj / new_array allocations are marked by a
    // stop-the-world mark-sweep collector, the roots are scanned conservatively (execution stacks of registered root
    // sources and the native thread stack), and the payloads of the objects according to the scan info of their type (the
    // type resolver tells which parts hold references; the objects that hold none are atomic: they are marked and never
    // scanned). alloc / free keep manual semantics, exactly like default_rt_heap_t.
    class gc_heap_t : public object_t, public rt_heap_t, public april_gc::gc_space_t
    {
        typedef april_gc::gc_space_t    __space_t;

    public:

        // Constructor.
        gc_heap_t(const gc_params_t & params);

        // Destructor.
        virtual ~gc_heap_t() override;

        // Creates a new object.
        virtual rt_ref_t new_obj(rt_type_t * type) override;

        // Creates a new array.
        virtual rt_ref_t new_array(rt_array_type_t * type, const array_length_t * lengths) override;

        // Creates a space that holds no references (garbage collected, never scanned).
        virtual void * new_atomic(size_t size) override;

        // Allocates a raw memory space (manual, not garbage collected).
        virtual void * alloc(size_t size, void * realloc = nullptr) override;

        // Frees a raw memory space.
        virtual void free(void * ptr) override;

        // Registers a gc root source.
        virtual void add_root_source(gc_root_source_t * source) override;

        // Removes a gc root source.
        virtual void remove_root_source(gc_root_source_t * source) override;

        // Sets the gc type resolver, clears the cached scan infos.
        virtual void set_gc_type_resolver(const gc_type_resolver_t & resolver) override;

        // Executes a garbage collection, returns bytes collected.
        virtual size_t collect() override;

        virtual size_t total_memory() override { return memory_in_use(); }
        virtual size_t collection_count() override { return collection_total(); }

        virtual uint64_t weak_new(void * object) override { return __space_t::weak_new(object); }
        virtual void *   weak_get(uint64_t handle) override { return __space_t::weak_get(handle); }
        virtual void     weak_free(uint64_t handle) override { __space_t::weak_free(handle); }

        virtual void *   take_finalizable() override { return __space_t::take_finalizable(); }
        virtual void     suppress_finalize(void * object, bool suppress) override;
        virtual void     set_finalizer_notify(const std::function<void ()> & notify) override { __notify = notify; }

        X_TO_STRING_IMPL(_T("gc_heap_t"))

        size_t verify_reports()  const { return __verify_reports; }

    protected:

        virtual void mark_roots(void ** native_stack_top) override;
        virtual void scan_object(const obj_t & obj) override;
        virtual void on_collected() override;
        virtual void out_of_memory() override;

    private:

        // Returns the scan info of an object (cached, resolved lazily).
        const gc_scan_info_t & __scan_info_of(rt_type_t * type);

        // Verify mode: reports skipped words that point to gc objects.
        void __verify_skipped(const obj_t & obj, rt_type_t * type, void ** begin, void ** end);

        gc_params_t     __params;

        // Registered root sources (execution contexts).
        std::vector<gc_root_source_t *> __root_sources;

        // Type resolver and the cache of resolved scan infos (the second one is the first level: it is by the address).
        gc_type_resolver_t                              __resolver;
        std::unordered_map<rt_type_t *, gc_scan_info_t> __scan_infos;
        gc_scan_info_t                                  __conservative_info;
        gc_scan_info_t                                  __unaligned_info;

        struct __cache_entry_t
        {
            rt_type_t *             type = nullptr;
            const gc_scan_info_t *  info = nullptr;
        };

        static constexpr size_t __cache_size = 1024;
        __cache_entry_t         __cache[__cache_size];

        std::function<void ()>  __notify;

        size_t          __verify_reports = 0;       // Suspicious skipped words found.
    };

}   // namespace X_ROOT_NS::modules::mm

#endif  // __MM_GC_HEAP_H__
