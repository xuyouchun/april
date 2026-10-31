
#ifndef __EXEC_GC_STACK_H__
#define __EXEC_GC_STACK_H__

#include <algorithm>
#include <cstddef>
#include <vector>

namespace X_ROOT_NS::modules::exec {

    ////////// ////////// ////////// ////////// //////////

    // Enumerates the ranges of an execution stack that the gc must scan: the whole
    // stack [bottom, top), minus the value-only regions of local variables of every
    // calling frame. It is a template over the stub / method types, so that it can be
    // tested without the interpreter.
    //
    //   stub_t   { unit_t * lp; void * current; method_t * method; }
    //            a calling stub sits right below the local variables of its frame;
    //            current == nullptr marks the bottom frame.
    //   method_t { size_t gc_value_begin, gc_value_end; }  // units from lp
    //
    // The frames are walked through the calling stubs. Whatever looks inconsistent
    // (a frame outside the stack, frames not strictly descending, regions that are
    // not nested properly) falls back to scanning the whole stack: skipping is only
    // an optimization, never worth a lost reference.
    template<typename stub_t, typename unit_t, typename f_t>
    void each_gc_stack_range(unit_t * bottom, unit_t * top, unit_t * lp, f_t f)
    {
        struct __hole_t { unit_t * begin, * end; };

        std::vector<__hole_t> holes;        // Innermost frame first: descending addresses.

        const size_t stub_units = sizeof(stub_t) / sizeof(unit_t);

        unit_t * limit = top;               // Regions of a frame lie below this.
        bool consistent = true;

        for (;;)
        {
            if (lp < bottom + stub_units || lp > top)
            {
                consistent = false;
                break;
            }

            stub_t * stub = (stub_t *)lp - 1;
            auto * method = stub->method;

            if (method != nullptr && method->gc_value_begin < method->gc_value_end)
            {
                unit_t * begin = lp + method->gc_value_begin;
                unit_t * end   = lp + method->gc_value_end;

                if (begin < bottom || end > limit)
                {
                    consistent = false;
                    break;
                }

                holes.push_back(__hole_t { begin, end });
                limit = begin;
            }

            if (stub->current == nullptr)   // The bottom frame.
                break;

            if (stub->lp >= lp)             // Frames must go down.
            {
                consistent = false;
                break;
            }

            limit = std::min(limit, (unit_t *)stub);
            lp = stub->lp;
        }

        if (!consistent)
        {
            f(bottom, top);
            return;
        }

        // The complement of the holes.
        unit_t * cursor = bottom;
        for (auto it = holes.rbegin(); it != holes.rend(); it++)
        {
            if (cursor < it->begin)
                f(cursor, it->begin);

            cursor = it->end;
        }

        if (cursor < top)
            f(cursor, top);
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec

#endif  // __EXEC_GC_STACK_H__
