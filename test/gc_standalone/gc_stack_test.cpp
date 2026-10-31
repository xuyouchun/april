// Standalone tests for exec/__gc_stack.h (the frame walker of gc stack scanning).
#define X_ROOT_NS april
#include "../../modules/exec/__gc_stack.h"

#include <cstdint>
#include <cstdio>
#include <vector>

using namespace april::modules::exec;

static int failures = 0, checks = 0;
#define CHECK(cond) do { checks++; \
    if (!(cond)) { std::printf("FAIL %d: %s\n", __LINE__, #cond); failures++; } \
} while (0)

typedef uint64_t unit_t;

struct method_t { size_t gc_value_begin, gc_value_end, unit_size; };
struct stub_t   { unit_t * lp; void * current; method_t * method; };   // 3 units

// Builds a stack the way command_execute_context_t does.
struct fake_stack_t
{
    std::vector<unit_t> buffer = std::vector<unit_t>(256, 0);
    unit_t * top;
    unit_t * lp;
    int      commands = 0;                  // Non-null marker for `current`.

    fake_stack_t() { top = lp = buffer.data(); }

    void push_calling(method_t * m, bool bottom = false)
    {
        stub_t * stub = (stub_t *)top;
        stub->lp      = lp;
        stub->current = bottom ? nullptr : &commands;
        stub->method  = m;

        top += sizeof(stub_t) / sizeof(unit_t);
        lp = top;
        if (m != nullptr) top += m->unit_size;
    }

    void push_temps(size_t n) { top += n; }

    // Which units does the walker ask to scan?
    std::vector<bool> scanned()
    {
        std::vector<bool> covered(buffer.size(), false);
        each_gc_stack_range<stub_t>(buffer.data(), top, lp,
            [&](unit_t * b, unit_t * e) {
                for (unit_t * p = b; p < e; p++) covered[p - buffer.data()] = true;
            });
        return covered;
    }

    size_t index(unit_t * p) { return p - buffer.data(); }
};

int main()
{
    // 1. Only the bottom frame: everything is scanned.
    {
        fake_stack_t s;
        s.push_calling(nullptr, true);
        s.push_temps(4);
        auto c = s.scanned();
        for (unit_t * p = s.buffer.data(); p < s.top; p++) CHECK(c[s.index(p)]);
        CHECK(!c[s.index(s.top)]);                       // above top: never scanned
    }

    // 2. Nested frames with value regions: exactly the regions are skipped.
    {
        fake_stack_t s;
        method_t mA { 2, 4, 6 }, mB { 1, 3, 5 }, mEmpty { 0, 0, 3 };

        s.push_calling(nullptr, true);
        s.push_calling(&mA);       unit_t * lpA = s.lp;
        s.push_temps(2);                                  // args / temps of A
        s.push_calling(&mEmpty);                          // a frame without a value region
        s.push_temps(1);
        s.push_calling(&mB);       unit_t * lpB = s.lp;
        s.push_temps(3);

        auto c = s.scanned();
        for (unit_t * p = s.buffer.data(); p < s.top; p++)
        {
            bool in_hole = (p >= lpA + 2 && p < lpA + 4) || (p >= lpB + 1 && p < lpB + 3);
            CHECK(c[s.index(p)] == !in_hole);
        }
    }

    // 3. A corrupt chain (stub->lp points upward) falls back to the whole stack.
    {
        fake_stack_t s;
        method_t mA { 2, 4, 6 };
        s.push_calling(nullptr, true);
        s.push_calling(&mA);
        s.push_temps(2);
        s.push_calling(&mA);
        ((stub_t *)s.lp - 1)->lp = s.lp + 5;             // upward
        auto c = s.scanned();
        for (unit_t * p = s.buffer.data(); p < s.top; p++) CHECK(c[s.index(p)]);
    }

    // 4. A hole that would overlap an inner frame (bogus region) falls back as well.
    {
        fake_stack_t s;
        method_t bogus { 1, 100, 6 };                    // end far beyond the frame
        s.push_calling(nullptr, true);
        s.push_calling(&bogus);
        s.push_calling(nullptr);
        s.push_temps(2);
        auto c = s.scanned();
        for (unit_t * p = s.buffer.data(); p < s.top; p++) CHECK(c[s.index(p)]);
    }

    // 5. lp outside of the stack falls back too (no crash, no skipped unit).
    {
        fake_stack_t s;
        method_t mA { 2, 4, 6 };
        s.push_calling(nullptr, true);
        s.push_calling(&mA);
        s.lp = s.buffer.data();                          // before the first stub
        auto c = s.scanned();
        for (unit_t * p = s.buffer.data(); p < s.top; p++) CHECK(c[s.index(p)]);
    }

    // 6. Deep recursion: 200 frames, every hole skipped, nothing else.
    {
        fake_stack_t s;
        s.buffer.assign(4096, 0); s.top = s.lp = s.buffer.data();
        method_t m { 1, 2, 3 };
        s.push_calling(nullptr, true);
        std::vector<unit_t *> lps;
        for (int i = 0; i < 200; i++) { s.push_calling(&m); lps.push_back(s.lp); s.push_temps(1); }
        auto c = s.scanned();
        size_t skipped = 0;
        for (unit_t * p = s.buffer.data(); p < s.top; p++) if (!c[s.index(p)]) skipped++;
        CHECK(skipped == 200);
        for (unit_t * lp : lps) CHECK(!c[s.index(lp + 1)]);
    }

    std::printf("%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
