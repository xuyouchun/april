// Standalone behavioral tests for april gc_heap_t (shim environment).
#include <mm.h>
#include "../../modules/mm/__gc_heap.h"
#include <cstdio>
#include <cstring>
#include <vector>

using namespace april;
using namespace april::modules::rt;
using namespace april::modules::mm;

static int failures = 0;
static int checks = 0;

#define CHECK(cond) do { checks++; \
    if (!(cond)) { std::printf("FAIL %d: %s\n", __LINE__, #cond); failures++; } \
} while (0)

// Simulated execution context (conservative stack range + precise roots).
class fake_root_source_t : public gc_root_source_t
{
public:
    std::vector<void *>   stack  = std::vector<void *>(64, nullptr);
    std::vector<rt_ref_t> precise;

    virtual void each_gc_root_ranges(
            const std::function<void (void **, void **)> & f) override
    {
        f(stack.data(), stack.data() + stack.size());
    }

    virtual void each_gc_roots(const std::function<void (rt_ref_t)> & f) override
    {
        for (rt_ref_t r : precise)
            f(r);
    }
};

static rt_heap_t * make_heap(bool stress = false)
{
    gc_params_t ps;
    ps.scan_native_stack = false;           // deterministic tests
    ps.initial_threshold = SIZE_MAX;        // only explicit collects
    ps.stress = stress;
    return create_gc_heap(nullptr, ps);
}

int main()
{
    rt_type_t t16(16), t4(4);

    // ---- 1. basic allocation & layout
    {
        rt_heap_t * heap = make_heap();
        fake_root_source_t roots;
        heap->add_root_source(&roots);

        rt_ref_t o = heap->new_obj(&t16);
        CHECK((void *)o != nullptr);
        CHECK(((uintptr_t)(void *)o % 8) == 0);
        CHECK(get_object_type(o) == &t16);
        std::memset((void *)o, 0xAB, 16);               // full payload writable

        // zero-initialized payload on a fresh cell
        rt_ref_t o2 = heap->new_obj(&t16);
        bool all_zero = true;
        for (int i = 0; i < 16; i++)
            all_zero &= (((byte_t *)(void *)o2)[i] == 0);
        CHECK(all_zero);

        // ---- 2. retention via conservative stack root, then reclaim
        roots.stack[0] = (void *)o;
        roots.stack[1] = (void *)o2;
        size_t collected = heap->collect();
        CHECK(collected == 0);
        CHECK(get_object_type(o) == &t16);              // header intact
        CHECK(((byte_t *)(void *)o)[7] == 0xAB);        // payload intact

        roots.stack[0] = nullptr;
        roots.stack[1] = nullptr;
        collected = heap->collect();
        CHECK(collected >= 2 * 24);                     // both cells reclaimed

        // ---- 3. interior pointer keeps the object alive
        rt_ref_t o3 = heap->new_obj(&t16);
        std::memset((void *)o3, 0xCD, 16);
        roots.stack[0] = (byte_t *)(void *)o3 + 12;     // middle of payload
        heap->collect();
        CHECK(get_object_type(o3) == &t16);
        CHECK(((byte_t *)(void *)o3)[0] == 0xCD);
        roots.stack[0] = nullptr;

        // ---- 4. object graph: a -> b, only a rooted
        rt_ref_t b = heap->new_obj(&t16);
        rt_ref_t a = heap->new_obj(&t16);
        *(void **)(void *)a = (void *)b;                // a.field0 = b
        std::memset((byte_t *)(void *)b + 0, 0x5A, 16);
        roots.stack[0] = (void *)a;
        size_t c1 = heap->collect();
        CHECK(get_object_type(a) == &t16);
        CHECK(get_object_type(b) == &t16);              // kept through a's payload
        CHECK(((byte_t *)(void *)b)[15] == 0x5A);
        roots.stack[0] = nullptr;
        size_t c2 = heap->collect();
        CHECK(c2 >= 2 * 24);

        // ---- 5. precise roots (exception chain analog)
        rt_ref_t e = heap->new_obj(&t16);
        roots.precise.push_back(e);
        heap->collect();
        CHECK(get_object_type(e) == &t16);
        roots.precise.clear();

        heap->remove_root_source(&roots);
    }

    // ---- 6. 1-D arrays
    {
        rt_heap_t * heap = make_heap();
        fake_root_source_t roots;
        heap->add_root_source(&roots);

        rt_array_type_t arr_t(1, &t4);
        array_length_t len = 100;
        rt_ref_t arr = heap->new_array(&arr_t, &len);

        CHECK(get_object_type(arr) == (rt_type_t *)&arr_t);
        CHECK(get_array_length(arr) == 100);

        int32_t * elems = (int32_t *)(void *)arr;
        for (int i = 0; i < 100; i++) elems[i] = i;     // ASAN validates bounds
        CHECK(elems[99] == 99);

        roots.stack[0] = (void *)arr;
        heap->collect();
        CHECK(get_array_length(arr) == 100);
        CHECK(elems[42] == 42);

        roots.stack[0] = nullptr;
        size_t c = heap->collect();
        CHECK(c >= 100 * 4);

        // ---- 7. multi-dimensional array [4][5][6]
        rt_array_type_t arr3_t(3, &t4);
        array_length_t lens[3] = { 4, 5, 6 };
        rt_ref_t arr3 = heap->new_array(&arr3_t, lens);

        CHECK(get_array_length(arr3) == 4 * 5 * 6);

        // partial products from the last dimension: obj-16=6, obj-20=30, obj-24=120
        array_length_t * partials = get_array_lengths(arr3);
        CHECK(partials[0] == 6);
        CHECK(partials[-1] == 30);
        CHECK(partials[-2] == 120);

        int32_t * e3 = (int32_t *)(void *)arr3;
        for (int i = 0; i < 120; i++) e3[i] = i;        // would overflow before the fix
        CHECK(e3[119] == 119);

        roots.stack[0] = (void *)arr3;
        heap->collect();
        CHECK(e3[119] == 119);
        roots.stack[0] = nullptr;
        heap->collect();

        heap->remove_root_source(&roots);
    }

    // ---- 8. large objects (> 8KB)
    {
        rt_heap_t * heap = make_heap();
        fake_root_source_t roots;
        heap->add_root_source(&roots);

        rt_array_type_t arr_t(1, &t4);
        array_length_t len = 10000;                     // 40KB payload
        rt_ref_t big = heap->new_array(&arr_t, &len);
        CHECK(get_array_length(big) == 10000);

        int32_t * elems = (int32_t *)(void *)big;
        for (int i = 0; i < 10000; i++) elems[i] = i;

        roots.stack[0] = (byte_t *)(void *)big + 20000; // interior pointer
        size_t c = heap->collect();
        CHECK(c == 0);
        CHECK(elems[9999] == 9999);

        roots.stack[0] = nullptr;
        c = heap->collect();
        CHECK(c >= 40000);

        heap->remove_root_source(&roots);
    }

    // ---- 9. stress mode: collect on every allocation, graph stays intact
    {
        rt_heap_t * heap = make_heap(true);
        fake_root_source_t roots;
        heap->add_root_source(&roots);

        rt_ref_t keep = heap->new_obj(&t16);
        roots.stack[0] = (void *)keep;

        for (int i = 0; i < 2000; i++)
        {
            rt_ref_t tmp = heap->new_obj(&t16);         // garbage each round
            *(void **)(void *)tmp = (void *)keep;
            (void)tmp;
        }

        CHECK(get_object_type(keep) == &t16);
        roots.stack[0] = nullptr;
        heap->remove_root_source(&roots);
    }

    // ---- 10. raw alloc/free keep manual semantics (never collected)
    {
        rt_heap_t * heap = make_heap();
        void * raw = heap->alloc(1024);
        CHECK(raw != nullptr);
        std::memset(raw, 0x11, 1024);
        heap->collect();                                // must not free raw
        CHECK(((byte_t *)raw)[1023] == 0x11);
        void * raw2 = heap->alloc(2048, raw);           // realloc semantics
        CHECK(raw2 != nullptr);
        heap->free(raw2);
    }


    // ---- 11. Phase 2a: precise scan infos
    {
        rt_type_t tNone(16), tRanges(24), tLeaf(16);
        rt_array_type_t arr_value_t(1, &t4);                  // elements: no refs

        auto make = [&](gc_type_resolver_t resolver, bool verify = false) {
            gc_params_t ps;
            ps.scan_native_stack = false;
            ps.initial_threshold = SIZE_MAX;
            ps.verify = verify;
            gc_heap_t * h = (gc_heap_t *)create_gc_heap(nullptr, ps);
            h->set_gc_type_resolver(resolver);
            return h;
        };

        // 11a. kind none: words that look like pointers are not followed.
        {
            gc_heap_t * heap = make([&](rt_type_t * t, gc_scan_info_t & info) {
                if (t == &tNone) { info.kind = gc_scan_info_t::kind_t::none; return true; }
                return false;
            });
            fake_root_source_t roots;
            heap->add_root_source(&roots);

            rt_ref_t holder = heap->new_obj(&tNone);
            rt_ref_t victim = heap->new_obj(&tLeaf);
            *(void **)(void *)holder = (void *)victim;       // looks like a ref, is data
            roots.stack[0] = (void *)holder;

            size_t c = heap->collect();
            CHECK(c == 32);                                  // victim reclaimed
            CHECK(get_object_type(holder) == &tNone);
            CHECK(heap->skipped_bytes() >= 16);
            heap->remove_root_source(&roots);
        }

        // 11b. kind ranges: refs at +0 and +16 followed, the word at +8 is not.
        {
            gc_heap_t * heap = make([&](rt_type_t * t, gc_scan_info_t & info) {
                if (t == &tRanges) {
                    info.kind = gc_scan_info_t::kind_t::ranges;
                    info.ranges = { {0, 8}, {16, 8} };
                    return true;
                }
                return false;
            });
            fake_root_source_t roots;
            heap->add_root_source(&roots);

            rt_ref_t a = heap->new_obj(&tLeaf), b = heap->new_obj(&tLeaf),
                     c = heap->new_obj(&tLeaf);
            rt_ref_t obj = heap->new_obj(&tRanges);
            void ** f = (void **)(void *)obj;
            f[0] = (void *)a;  f[1] = (void *)b;  f[2] = (void *)c;
            std::memset((void *)a, 0x77, 16);
            std::memset((void *)c, 0x99, 16);
            roots.stack[0] = (void *)obj;

            size_t collected = heap->collect();
            CHECK(collected == 32);                          // exactly b
            CHECK(get_object_type(a) == &tLeaf);
            CHECK(get_object_type(c) == &tLeaf);
            CHECK(((byte_t *)(void *)a)[5] == 0x77);
            CHECK(((byte_t *)(void *)c)[5] == 0x99);
            heap->remove_root_source(&roots);
        }

        // 11c. value arrays are skipped entirely, big ones included (large object).
        {
            gc_heap_t * heap = make([&](rt_type_t * t, gc_scan_info_t & info) {
                if (t == (rt_type_t *)&arr_value_t)
                    { info.kind = gc_scan_info_t::kind_t::none; return true; }
                return false;
            });
            fake_root_source_t roots;
            heap->add_root_source(&roots);

            array_length_t len = 20000;                      // 80KB: large object
            rt_ref_t arr = heap->new_array(&arr_value_t, &len);
            rt_ref_t victim = heap->new_obj(&tLeaf);
            ((void **)(void *)arr)[100] = (void *)victim;
            roots.stack[0] = (void *)arr;

            size_t c = heap->collect();
            CHECK(c == 32);
            CHECK(get_array_length(arr) == 20000);
            CHECK(heap->skipped_bytes() >= 80000);
            CHECK(heap->scanned_bytes() == 0);
            heap->remove_root_source(&roots);
        }

        // 11d. resolver returns false / throws: scanned conservatively.
        for (int mode = 0; mode < 2; mode++)
        {
            gc_heap_t * heap = make([&](rt_type_t * t, gc_scan_info_t & info) -> bool {
                if (mode == 0) return false;
                throw std::runtime_error("resolver failure");
            });
            fake_root_source_t roots;
            heap->add_root_source(&roots);

            rt_ref_t holder = heap->new_obj(&tNone);
            rt_ref_t kept = heap->new_obj(&tLeaf);
            *(void **)(void *)holder = (void *)kept;
            roots.stack[0] = (void *)holder;

            size_t c = heap->collect();
            CHECK(c == 0);
            CHECK(get_object_type(kept) == &tLeaf);
            heap->remove_root_source(&roots);
        }

        // 11e. ranges past the cell end are clipped (no out-of-bounds read; ASAN).
        {
            gc_heap_t * heap = make([&](rt_type_t * t, gc_scan_info_t & info) {
                if (t == &tLeaf) {
                    info.kind = gc_scan_info_t::kind_t::ranges;
                    info.ranges = { {0, 8}, {8, 100000} };
                    return true;
                }
                return false;
            });
            fake_root_source_t roots;
            heap->add_root_source(&roots);

            rt_ref_t o = heap->new_obj(&tLeaf);
            roots.stack[0] = (void *)o;
            heap->collect();
            CHECK(get_object_type(o) == &tLeaf);
            heap->remove_root_source(&roots);
        }

        // 11f. verify mode reports a wrong scan info instead of silently losing the ref.
        {
            gc_heap_t * heap = make([&](rt_type_t * t, gc_scan_info_t & info) {
                if (t == &tNone) { info.kind = gc_scan_info_t::kind_t::none; return true; }
                return false;
            }, true);
            fake_root_source_t roots;
            heap->add_root_source(&roots);

            rt_ref_t holder = heap->new_obj(&tNone);
            rt_ref_t real = heap->new_obj(&tLeaf);
            *(void **)(void *)holder = (void *)real;         // a real ref the info misses
            roots.stack[0] = (void *)holder;

            std::fprintf(stderr, "(expected: one [gc-verify] line follows)\n");
            heap->collect();
            CHECK(heap->verify_reports() == 1);
            heap->remove_root_source(&roots);
        }

        // 11g. objects stay valid across many cycles with a mixed resolver (stress).
        {
            gc_params_t ps;
            ps.scan_native_stack = false;
            ps.stress = true;
            ps.verify = true;
            gc_heap_t * heap = (gc_heap_t *)create_gc_heap(nullptr, ps);
            heap->set_gc_type_resolver([&](rt_type_t * t, gc_scan_info_t & info) {
                if (t == &tRanges) {
                    info.kind = gc_scan_info_t::kind_t::ranges;
                    info.ranges = { {0, 8}, {16, 8} };
                    return true;
                }
                return false;
            });
            fake_root_source_t roots;
            heap->add_root_source(&roots);

            rt_ref_t head = heap->new_obj(&tRanges);
            roots.stack[0] = (void *)head;
            rt_ref_t tail = head;

            for (int i = 0; i < 300; i++)                    // a linked list through f[0]
            {
                rt_ref_t node = heap->new_obj(&tRanges);
                ((void **)(void *)tail)[0] = (void *)node;
                ((int64_t *)(void *)node)[1] = i;            // value slot
                tail = node;
                roots.precise.push_back(tail);
                if (roots.precise.size() > 1) roots.precise.erase(roots.precise.begin());
            }

            int count = 0;
            for (void * p = (void *)head; p != nullptr; p = ((void **)p)[0])
            {
                CHECK(get_object_type(p) == &tRanges);
                count++;
                if (count > 1000) break;
            }
            CHECK(count == 301);
            CHECK(heap->verify_reports() == 0);
            roots.stack[0] = nullptr;
            heap->remove_root_source(&roots);
        }
    }

    std::printf("%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
