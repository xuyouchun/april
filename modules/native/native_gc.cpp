
#include "native_gc.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <csetjmp>
#include <map>
#include <vector>

// The garbage collector of native programs: a conservative, non moving mark and sweep.
//
// The blocks are registered by their address and size. A word that points into a block
// (also in its middle: the program holds the addresses of fields and elements) keeps it
// alive; the words of the live blocks are scanned the same way, so no knowledge of the
// types is needed. The roots are the native stack (and the registers, saved by setjmp)
// and the registered globals.
//
// Environment variables, for debugging and tests:
//
//      APRIL_GC_STRESS=1       collects at every allocation
//      APRIL_GC_THRESHOLD=n    collects when n bytes were allocated since the last time
//      APRIL_GC_TRACE=1        prints a line for every collection
//      APRIL_GC_STATS=1        prints a summary when the program ends

namespace april_native_gc {

    namespace
    {
        struct __block_t
        {
            size_t  size;
            bool    marked;
            bool    has_refs;       // false: the block holds no addresses, it is not scanned.
        };

        std::map<uintptr_t, __block_t>  __blocks;
        std::vector<void **>            __roots;
        std::vector<std::pair<uintptr_t, uintptr_t>>    __work;     // Ranges to scan.

        uintptr_t   __stack_base = 0;
        uintptr_t   __low = UINTPTR_MAX, __high = 0;        // Bounds of the blocks.

        size_t      __allocated_since = 0;      // Bytes allocated since the last collection.
        size_t      __live_bytes = 0;
        size_t      __peak_bytes = 0;
        size_t      __collections = 0;
        size_t      __threshold = 0;
        size_t      __min_threshold = 8u << 20;

        bool        __stress = false;
        bool        __trace = false;
        bool        __stats = false;

        bool __env_flag(const char * name)
        {
            const char * value = std::getenv(name);
            return value != nullptr && *value == '1';
        }

        // Marks the block that holds the address, if any.
        void __mark_word(uintptr_t word)
        {
            if (word < __low || word >= __high)
                return;

            auto it = __blocks.upper_bound(word);
            if (it == __blocks.begin())
                return;

            --it;
            __block_t & block = it->second;
            if (word >= it->first + block.size || block.marked)
                return;

            block.marked = true;
            if (block.has_refs)
                __work.push_back({ it->first, it->first + block.size });
        }

        void __scan(uintptr_t begin, uintptr_t end)
        {
            begin = (begin + 7) & ~(uintptr_t)7;
            for (uintptr_t p = begin; p + sizeof(uintptr_t) <= end; p += sizeof(uintptr_t))
                __mark_word(*(uintptr_t *)p);
        }

        void __drain()
        {
            while (!__work.empty())
            {
                auto range = __work.back();
                __work.pop_back();

                __scan(range.first, range.second);
            }
        }

        // The stack is scanned from here: not inlined, so that the registers are saved
        // below the frames of the caller.
        __attribute__((noinline)) void __collect_from_stack()
        {
            std::jmp_buf registers;
            setjmp(registers);

            volatile char marker = 0;
            uintptr_t low = (uintptr_t)&marker;
            uintptr_t regs = (uintptr_t)&registers;

            __scan(regs, regs + sizeof(registers));
            __scan(low < regs? low : regs, __stack_base);

            for (void ** slot : __roots)
                __mark_word((uintptr_t)*slot);

            __drain();
        }

        void __collect()
        {
            if (__stack_base == 0)
                return;                 // Not initialized.

            size_t before = __live_bytes;

            __collect_from_stack();

            // Sweeps.
            for (auto it = __blocks.begin(); it != __blocks.end(); )
            {
                if (it->second.marked)
                {
                    it->second.marked = false;
                    ++it;
                }
                else
                {
                    __live_bytes -= it->second.size;
                    std::free((void *)it->first);
                    it = __blocks.erase(it);
                }
            }

            __collections++;
            __allocated_since = 0;
            __threshold = __live_bytes > __min_threshold? __live_bytes : __min_threshold;

            if (__trace)
            {
                std::fprintf(stderr, "native gc: %zu bytes -> %zu bytes, %zu blocks\n",
                             before, __live_bytes, __blocks.size());
            }
        }
    }

    void init(void * stack_base)
    {
        __stack_base = (uintptr_t)stack_base;

        __stress = __env_flag("APRIL_GC_STRESS");
        __trace  = __env_flag("APRIL_GC_TRACE");
        __stats  = __env_flag("APRIL_GC_STATS");

        if (const char * value = std::getenv("APRIL_GC_THRESHOLD"))
        {
            if (*value != 0)
                __min_threshold = (size_t)std::strtoull(value, nullptr, 10);
        }

        __threshold = __min_threshold;
    }

    void shutdown()
    {
        if (__stats)
        {
            std::fprintf(stderr, "native gc: collections=%zu peak=%zu live=%zu\n",
                         __collections, __peak_bytes, __live_bytes);
        }
    }

    void add_root(void ** slot)
    {
        __roots.push_back(slot);
    }

    void * alloc(size_t size, bool has_refs)
    {
        if (__stress || __allocated_since >= __threshold)
            __collect();

        void * p = std::calloc(1, size);
        if (p == nullptr)
        {
            __collect();
            p = std::calloc(1, size);
        }

        if (p == nullptr)
        {
            std::fputs("native: out of memory\n", stderr);
            std::abort();
        }

        uintptr_t address = (uintptr_t)p;
        __blocks[address] = { size, false, has_refs };

        if (address < __low)
            __low = address;

        if (address + size > __high)
            __high = address + size;

        __allocated_since += size;
        __live_bytes += size;
        if (__live_bytes > __peak_bytes)
            __peak_bytes = __live_bytes;

        return p;
    }

}   // namespace april_native_gc
