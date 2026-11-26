
#include <gc_space.h>

#include <algorithm>
#include <chrono>
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

#if defined(_WIN32)

// Declared here instead of including <windows.h>, whose macros pollute the project.
extern "C" {
    __declspec(dllimport) void * __stdcall VirtualAlloc(void * address, size_t size, unsigned long type, unsigned long protect);
    __declspec(dllimport) int __stdcall VirtualFree(void * address, size_t size, unsigned long type);
    __declspec(dllimport) void __stdcall GetCurrentThreadStackLimits(unsigned long long * low, unsigned long long * high);
}

#elif defined(__linux__)
#include <pthread.h>
#include <sys/mman.h>
#else
#include <sys/mman.h>
#endif

namespace april_gc {

    ////////// ////////// ////////// ////////// //////////
    // block_t

    void block_t::initialize(uint16_t size_class_, uint32_t cell_size_, bool atomic_, bool dirty)
    {
        magic       = magic_value;
        size_class  = size_class_;
        atomic      = atomic_;
        cell_size   = cell_size_;
        cursor      = 0;
        mark_count  = 0;
        next        = nullptr;

        size_t header_size = (sizeof(block_t) + granule_size - 1) & ~(granule_size - 1);
        first_cell_granule = (uint32_t)(header_size / granule_size);
        cell_count = (uint32_t)((block_size - header_size) / cell_size_);
        reciprocal = (((uint64_t)1 << 32) / (cell_size_ / granule_size)) + 1;

        // A block that was committed now is all 0, the one that was used before is not.
        zero_from = dirty? cell_count : 0;

        std::memset(byte_map, 0, sizeof(byte_map));
    }

    ////////// ////////// ////////// ////////// //////////
    // The memory.

    namespace
    {
        constexpr size_t __page_size = 4096;

        // The blocks that are kept committed when they are free (they are used at once, with no call to the system).
        constexpr size_t __warm_blocks = 512;

        constexpr size_t __no_run = ~(size_t)0;

        // The time, in microseconds.
        typedef std::chrono::steady_clock __clock_t;

        size_t __micros(__clock_t::time_point a, __clock_t::time_point b)
        {
            return (size_t)std::chrono::duration_cast<std::chrono::microseconds>(b - a).count();
        }
    }

    bool gc_space_t::__reserve(size_t bytes)
    {
    #if defined(_WIN32)
        void * p = ::VirtualAlloc(nullptr, bytes, 0x2000 /* MEM_RESERVE */, 1 /* PAGE_NOACCESS */);
        if (p == nullptr)
            return false;

        __reservation = p;
        __base = (byte_t *)(((uintptr_t)p + block_size - 1) & ~(uintptr_t)(block_size - 1));
    #else
        void * p = ::mmap(nullptr, bytes + block_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
        if (p == MAP_FAILED)
            return false;

        __reservation = p;
        __base = (byte_t *)(((uintptr_t)p + block_size - 1) & ~(uintptr_t)(block_size - 1));
    #endif

        __capacity = (bytes - (size_t)(__base - (byte_t *)p)) / block_size;
        return true;
    }

    bool gc_space_t::__commit(void * address, size_t bytes)
    {
    #if defined(_WIN32)
        return ::VirtualAlloc(address, bytes, 0x1000 /* MEM_COMMIT */, 4 /* PAGE_READWRITE */) != nullptr;
    #else
        return ::mprotect(address, bytes, PROT_READ | PROT_WRITE) == 0;
    #endif
    }

    void gc_space_t::__decommit(void * address, size_t bytes)
    {
    #if defined(_WIN32)
        ::VirtualFree(address, bytes, 0x4000 /* MEM_DECOMMIT */);
    #else
        ::madvise(address, bytes, MADV_DONTNEED);
        ::mprotect(address, bytes, PROT_NONE);
    #endif
    }

    // Takes a run of blocks that were never used or were given back (they are not committed).
    size_t gc_space_t::__alloc_run(size_t count)
    {
        for (auto it = __free_runs.begin(); it != __free_runs.end(); ++it)
        {
            if (it->second >= count)
            {
                size_t start = it->first, length = it->second;
                __free_runs.erase(it);

                if (length > count)
                    __free_runs[start + count] = length - count;

                return start;
            }
        }

        if (__top + count > __capacity)
            return __no_run;

        size_t start = __top;
        __top += count;
        return start;
    }

    // Gives a run of blocks back to the system.
    void gc_space_t::__release_run(size_t start, size_t count)
    {
        __decommit(__block_address(start), count * block_size);

        for (size_t k = 0; k < count; k++)
        {
            __kind[start + k] = __k_free;
            __aux[start + k] = 0;
        }

        auto next = __free_runs.lower_bound(start);
        if (next != __free_runs.end() && start + count == next->first)
        {
            count += next->second;
            next = __free_runs.erase(next);
        }

        if (next != __free_runs.begin())
        {
            auto prev = std::prev(next);
            if (prev->first + prev->second == start)
            {
                start = prev->first;
                count += prev->second;
                __free_runs.erase(prev);
            }
        }

        if (start + count == __top)
            __top = start;                  // The end of the used region goes back.
        else
            __free_runs[start] = count;
    }

    ////////// ////////// ////////// ////////// //////////
    // gc_space_t

    gc_space_t::gc_space_t(const space_params_t & params)
        : __params(params)
    {
        __threshold = params.initial_threshold;

        std::memset(__current, 0, sizeof(__current));
        std::memset(__head, 0, sizeof(__head));

        size_t size_class = 0;
        for (size_t index = 0, count = max_cell_size / granule_size; index < count; index++)
        {
            size_t cell_size = (index + 1) * granule_size;
            while (cell_sizes[size_class] < cell_size)
                size_class++;

            __class_map[index] = (byte_t)size_class;
        }

        // The address space: as much as the system gives, up to 32 GB.
        size_t bytes = params.reserve_bytes != 0? params.reserve_bytes : ((size_t)32 << 30);
        bool reserved = false;

        for (; bytes >= ((size_t)256 << 20); bytes >>= 1)
        {
            if (__reserve(bytes))
            {
                reserved = true;
                break;
            }
        }

        if (!reserved)
            throw std::bad_alloc();

        __kind.assign(__capacity, __k_free);
        __aux.assign(__capacity, 0);
    }

    gc_space_t::~gc_space_t()
    {
        if (__params.stats)
        {
            std::fprintf(stderr,
                "[gc] collections %llu, pause total %.2f ms (mark %.2f, sweep %.2f), max %.2f ms, collected %llu bytes\n",
                (unsigned long long)__collections, __total_pause_us / 1000.0, __total_mark_us / 1000.0,
                __total_sweep_us / 1000.0, __max_pause_us / 1000.0, (unsigned long long)__total_collected
            );
        }

        if (__reservation != nullptr)
        {
        #if defined(_WIN32)
            ::VirtualFree(__reservation, 0, 0x8000 /* MEM_RELEASE */);
        #else
            ::munmap(__reservation, __capacity * block_size + block_size);
        #endif
        }
    }

    void gc_space_t::out_of_memory()
    {
        throw std::bad_alloc();
    }

    ////////// ////////// ////////// ////////// //////////
    // Allocation.

    void * gc_space_t::allocate(size_t extern_size, size_t size, bool atomic)
    {
        if (__params.stress || __allocated_since_gc >= __threshold)
            request_collect();

        // The address of the object is cell + extern_size: it is inside the cell, else a reference to an empty array (its
        // type and length, no elements) points to the next cell, and the array is freed.
        if (size == 0)
            size = 1;

        size_t cell_size = (extern_size + size + granule_size - 1) & ~(granule_size - 1);

        for (int attempt = 0; ; attempt++)
        {
            void * cell;
            size_t counted;

            if (cell_size <= max_cell_size && extern_size <= max_small_extern)
            {
                size_t size_class = __size_class_of(cell_size);
                counted = cell_sizes[size_class];
                cell = __alloc_cell(size_class, atomic, (byte_t)(1 + extern_size / 8));
            }
            else
            {
                counted = extern_size + ((size + granule_size - 1) & ~(granule_size - 1));
                cell = __alloc_large(extern_size, size, atomic);
            }

            if (cell != nullptr)
            {
                __allocated_since_gc += counted;
                __in_use += counted;

                return (byte_t *)cell + extern_size;
            }

            // The space is full: a collection may free blocks.
            if (attempt > 0)
                out_of_memory();

            request_collect();
        }
    }

    block_t * gc_space_t::__new_block(size_t size_class, bool atomic)
    {
        size_t index;
        bool dirty;

        if (!__warm.empty())
        {
            index = __warm.back();
            __warm.pop_back();
            dirty = true;
        }
        else
        {
            index = __alloc_run(1);
            if (index == __no_run)
                return nullptr;

            if (!__commit(__block_address(index), block_size))
            {
                __release_run(index, 1);
                return nullptr;
            }

            dirty = false;
        }

        __kind[index] = __k_small;
        __small_blocks.push_back(index);

        block_t * block = (block_t *)__block_address(index);
        block->initialize((uint16_t)size_class, cell_sizes[size_class], atomic, dirty);

        return block;
    }

    void gc_space_t::__release_block(size_t index)
    {
        __kind[index] = __k_free;

        if (__warm.size() < __warm_blocks)
            __warm.push_back(index);
        else
            __release_run(index, 1);
    }

    void * gc_space_t::__alloc_cell(size_t size_class, bool atomic, byte_t entry)
    {
        block_t *& current = __current[atomic][size_class];

        for (;;)
        {
            if (current != nullptr)
            {
                block_t * block = current;
                const uint32_t count = block->cell_count;
                const size_t stride = block->cell_size / granule_size;
                byte_t * e = block->byte_map + block->first_cell_granule + (size_t)block->cursor * stride;

                for (uint32_t index = block->cursor; index < count; index++, e += stride)
                {
                    if ((*e & used_mask) != 0)
                        continue;

                    *e = entry;
                    block->cursor = index + 1;

                    byte_t * cell = block->cell_at(index);

                    if (index >= block->zero_from)
                        block->zero_from = index + 1;           // Never used: it is 0 already.
                    else
                        std::memset(cell, 0, block->cell_size);

                    return cell;
                }

                block->cursor = count;
                current = nullptr;
            }

            block_t *& head = __head[atomic][size_class];
            if (head != nullptr)
            {
                current = head;
                head = head->next;
                current->next = nullptr;
                continue;
            }

            current = __new_block(size_class, atomic);
            if (current == nullptr)
                return nullptr;
        }
    }

    void * gc_space_t::__alloc_large(size_t extern_size, size_t size, bool atomic)
    {
        size_t total = extern_size + ((size + granule_size - 1) & ~(granule_size - 1));
        size_t bytes = large_t::header_size + total;
        size_t count = (bytes + block_size - 1) / block_size;

        size_t index = __alloc_run(count);
        if (index == __no_run)
            return nullptr;

        // Only the pages that the object uses.
        if (!__commit(__block_address(index), (bytes + __page_size - 1) & ~(__page_size - 1)))
        {
            __release_run(index, count);
            return nullptr;
        }

        __kind[index] = __k_large_head;
        __aux[index] = (uint32_t)count;

        for (size_t k = 1; k < count; k++)
        {
            __kind[index + k] = __k_large_cont;
            __aux[index + k] = (uint32_t)k;
        }

        large_t * large = (large_t *)__block_address(index);
        large->total_size   = total;
        large->extern_size  = (uint32_t)extern_size;
        large->block_count  = (uint32_t)count;
        large->marked       = false;
        large->atomic       = atomic;

        __large_heads.push_back(index);
        return large->payload();
    }

    void gc_space_t::__free_large(size_t index)
    {
        large_t * large = (large_t *)__block_address(index);
        __release_run(index, large->block_count);
    }

    ////////// ////////// ////////// ////////// //////////
    // Locating and marking.

    bool gc_space_t::locate(void * p, obj_t & out)
    {
        size_t off = (size_t)((uintptr_t)p - (uintptr_t)__base);
        if (p == nullptr || off >= __top * block_size)
            return false;

        size_t bi = off >> 16;
        byte_t kind = __kind[bi];

        if (kind == __k_small)
        {
            block_t * block = (block_t *)__block_address(bi);

            size_t granule = (off & (block_size - 1)) / granule_size;
            if (granule < block->first_cell_granule)
                return false;

            size_t index = block->index_of(granule);
            if (index >= block->cell_count)
                return false;

            byte_t & entry = block->byte_map[block->granule_of(index)];
            if ((entry & used_mask) == 0)               // A free cell.
                return false;

            out.cell        = block->cell_at(index);
            out.size        = block->cell_size;
            out.extern_size = (uint32_t)((entry & used_mask) - 1) * 8;
            out.atomic      = block->atomic;
            out.mark_byte   = &entry;
            out.large       = nullptr;
            out.block       = block;

            return true;
        }

        if (kind == __k_large_head || kind == __k_large_cont)
        {
            size_t head = kind == __k_large_cont? bi - __aux[bi] : bi;
            large_t * large = (large_t *)__block_address(head);
            byte_t * payload = large->payload();

            if ((byte_t *)p < payload || (byte_t *)p >= payload + large->total_size)
                return false;

            out.cell        = payload;
            out.size        = large->total_size;
            out.extern_size = large->extern_size;
            out.atomic      = large->atomic;
            out.mark_byte   = nullptr;
            out.large       = large;
            out.block       = nullptr;

            return true;
        }

        return false;
    }

    void gc_space_t::mark_candidate(void * p)
    {
        size_t off = (size_t)((uintptr_t)p - (uintptr_t)__base);
        if (off >= __top * block_size)
            return;

        size_t bi = off >> 16;
        byte_t kind = __kind[bi];

        if (kind == __k_small)
        {
            block_t * block = (block_t *)__block_address(bi);

            size_t granule = (off & (block_size - 1)) / granule_size;
            if (granule < block->first_cell_granule)
                return;

            size_t index = block->index_of(granule);
            if (index >= block->cell_count)
                return;

            byte_t & entry = block->byte_map[block->granule_of(index)];
            if ((entry & used_mask) == 0 || (entry & mark_mask) != 0)
                return;

            entry |= mark_mask;
            block->mark_count++;

            // An atomic object holds no references: it is marked, there is nothing to scan.
            if (!block->atomic)
                __mark_stack.push_back({ block->cell_at(index), block->cell_size, (uint32_t)((entry & used_mask) - 1) * 8 });

            return;
        }

        if (kind == __k_large_head || kind == __k_large_cont)
        {
            size_t head = kind == __k_large_cont? bi - __aux[bi] : bi;
            large_t * large = (large_t *)__block_address(head);
            byte_t * payload = large->payload();

            if ((byte_t *)p < payload || (byte_t *)p >= payload + large->total_size || large->marked)
                return;

            large->marked = true;

            if (!large->atomic)
                __mark_stack.push_back({ payload, large->total_size, large->extern_size });
        }
    }

    bool gc_space_t::is_marked(void * p)
    {
        obj_t obj;
        if (!locate(p, obj))
            return true;

        return obj.large != nullptr? obj.large->marked : (*obj.mark_byte & mark_mask) != 0;
    }

    void gc_space_t::scan_range(void ** begin, void ** end, bool unaligned)
    {
        if (unaligned)
        {
            // The words are at any multiple of 4 (an unaligned read is fine on the 64 bits platforms we run on).
            byte_t * p = (byte_t *)(((uintptr_t)begin + 3) & ~(uintptr_t)3);
            byte_t * stop = (byte_t *)end;

            for (; p + sizeof(void *) <= stop; p += 4)
            {
                void * word;
                std::memcpy(&word, p, sizeof(word));
                mark_candidate(word);
            }

            return;
        }

        begin = (void **)(((uintptr_t)begin + sizeof(void *) - 1) & ~(sizeof(void *) - 1));

        for (void ** p = begin; p < end; p++)
            mark_candidate(*p);
    }

    void gc_space_t::scan_object(const obj_t & obj)
    {
        scanned_bytes_ += obj.size - obj.extern_size;
        scan_range((void **)(obj.cell + obj.extern_size), (void **)(obj.cell + obj.size));
    }

    void gc_space_t::process_mark_stack()
    {
        while (!__mark_stack.empty())
        {
            __queued_t q = __mark_stack.back();
            __mark_stack.pop_back();

            obj_t obj;
            obj.cell        = q.cell;
            obj.size        = q.size;
            obj.extern_size = q.extern_size;
            obj.atomic      = false;
            obj.mark_byte   = nullptr;
            obj.large       = nullptr;
            obj.block       = nullptr;

            scan_object(obj);
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // The weak references and the finalizers.

    uint64_t gc_space_t::weak_new(void * object)
    {
        uint64_t index;

        if (!__free_weaks.empty())
        {
            index = __free_weaks.back();
            __free_weaks.pop_back();
            __weaks[index] = (uintptr_t)object;
        }
        else
        {
            index = __weaks.size();
            __weaks.push_back((uintptr_t)object);
        }

        return index + 1;
    }

    void * gc_space_t::weak_get(uint64_t handle)
    {
        if (handle == 0 || handle > __weaks.size())
            return nullptr;

        return (void *)__weaks[handle - 1];
    }

    void gc_space_t::weak_free(uint64_t handle)
    {
        if (handle == 0 || handle > __weaks.size())
            return;

        __weaks[handle - 1] = 0;
        __free_weaks.push_back(handle - 1);
    }

    void gc_space_t::register_finalizable(void * object)
    {
        __fin_registry.insert((uintptr_t)object);
    }

    bool gc_space_t::unregister_finalizable(void * object)
    {
        return __fin_registry.erase((uintptr_t)object) > 0;
    }

    void * gc_space_t::take_finalizable()
    {
        if (__fin_queue.empty())
            return nullptr;

        uintptr_t object = __fin_queue.front();
        __fin_queue.erase(__fin_queue.begin());

        return (void *)object;
    }

    ////////// ////////// ////////// ////////// //////////
    // The collection.

    size_t gc_space_t::collect_here()
    {
        // Spills the callee-saved registers onto the native stack, so that the references that are held only in registers
        // are seen by the scan.
        std::jmp_buf registers;
        setjmp(registers);

        void ** native_stack_top = (void **)&registers;
        return __collect(native_stack_top);
    }

    size_t gc_space_t::__collect(void ** native_stack_top)
    {
        __mark_stack.clear();
        scanned_bytes_ = 0;
        skipped_bytes_ = 0;

        auto t0 = __clock_t::now();

        mark_roots(native_stack_top);

        // The objects that wait for their finalizer are alive until it ran.
        for (uintptr_t p : __fin_queue)
            mark_candidate((void *)p);

        process_mark_stack();

        // The weak references of the objects that nothing reaches are cleared (before the finalizers bring some back).
        for (uintptr_t & w : __weaks)
        {
            if (w != 0 && !is_marked((void *)w))
                w = 0;
        }

        // The objects that have a finalizer and are not reachable: the finalizer is asked, the object (and what it holds)
        // lives until it ran.
        if (!__fin_registry.empty())
        {
            std::vector<uintptr_t> dead;
            for (uintptr_t p : __fin_registry)
            {
                if (!is_marked((void *)p))
                    dead.push_back(p);
            }

            for (uintptr_t p : dead)
            {
                __fin_registry.erase(p);
                __fin_queue.push_back(p);
                mark_candidate((void *)p);
            }

            if (!dead.empty())
                process_mark_stack();
        }

        on_marked();

        auto t1 = __clock_t::now();

        size_t collected = __sweep();

        auto t2 = __clock_t::now();

        size_t mark_us = __micros(t0, t1), sweep_us = __micros(t1, t2);
        __collections++;
        __total_mark_us += mark_us;
        __total_sweep_us += sweep_us;
        __total_pause_us += mark_us + sweep_us;
        __total_collected += collected;
        if (mark_us + sweep_us > __max_pause_us)
            __max_pause_us = mark_us + sweep_us;

        __allocated_since_gc = 0;
        __threshold = std::max(
            __params.initial_threshold, (size_t)((double)__live_bytes * __params.growth_factor)
        );

        if (__params.trace)
        {
            // %llu with casts: the C library of Windows (msvcrt) does not know %zu.
            std::fprintf(stderr,
                "[gc] #%llu collected %llu, live %llu, payload scanned %llu / skipped %llu, "
                "next threshold %llu, mark %.2f ms, sweep %.2f ms\n",
                (unsigned long long)__collections, (unsigned long long)collected,
                (unsigned long long)__live_bytes, (unsigned long long)scanned_bytes_,
                (unsigned long long)skipped_bytes_, (unsigned long long)__threshold,
                mark_us / 1000.0, sweep_us / 1000.0
            );
        }

        on_collected();
        return collected;
    }

    size_t gc_space_t::__sweep()
    {
        size_t live = 0;

        std::memset(__current, 0, sizeof(__current));
        std::memset(__head, 0, sizeof(__head));

        block_t * tails[2][size_class_count];
        std::memset(tails, 0, sizeof(tails));

        std::vector<size_t> kept;
        kept.reserve(__small_blocks.size());

        for (size_t index : __small_blocks)
        {
            block_t * block = (block_t *)__block_address(index);

            if (block->mark_count == 0)                     // Nothing lives: the block is free.
            {
                if (__params.poison)
                    std::memset(block->cell_at(0), 0xDD, (size_t)block->cell_count * block->cell_size);

                __release_block(index);
                continue;
            }

            const uint32_t count = block->cell_count;
            const size_t stride = block->cell_size / granule_size;
            uint32_t used = 0;

            // The byte map only: the cells that are free are found when they are allocated.
            byte_t * e = block->byte_map + block->first_cell_granule;
            for (uint32_t k = 0; k < count; k++, e += stride)
            {
                byte_t v = *e;

                if (v & mark_mask)
                {
                    *e = v & used_mask;
                    used++;
                }
                else if (v != 0)
                {
                    *e = 0;

                    if (__params.poison)
                        std::memset(block->cell_at(k), 0xDD, block->cell_size);
                }
            }

            block->cursor = 0;
            block->mark_count = 0;
            block->next = nullptr;
            live += (size_t)used * block->cell_size;

            kept.push_back(index);

            if (used < count)
            {
                block_t *& tail = tails[block->atomic][block->size_class];
                if (tail == nullptr)
                    __head[block->atomic][block->size_class] = block;
                else
                    tail->next = block;

                tail = block;
            }
        }

        __small_blocks.swap(kept);

        std::vector<size_t> kept_large;
        for (size_t index : __large_heads)
        {
            large_t * large = (large_t *)__block_address(index);

            if (large->marked)
            {
                large->marked = false;
                live += large->total_size;
                kept_large.push_back(index);
            }
            else
            {
                if (__params.poison)
                    std::memset(large->payload(), 0xDD, large->total_size);

                __free_large(index);
            }
        }

        __large_heads.swap(kept_large);

        // A few free blocks stay committed.
        while (__warm.size() > __warm_blocks)
        {
            size_t index = __warm.back();
            __warm.pop_back();
            __release_run(index, 1);
        }

        size_t collected = __in_use > live? __in_use - live : 0;
        __in_use = live;
        __live_bytes = live;

        return collected;
    }

    ////////// ////////// ////////// ////////// //////////

    // Returns the high bound of the stack of the thread.
    void ** gc_space_t::native_stack_high()
    {
        // Each thread has its own stack: the bound is kept by the thread.
        static thread_local void ** __stack_high = nullptr;

        if (__stack_high == nullptr)
        {
        #if defined(__linux__)
            pthread_attr_t attr;
            if (pthread_getattr_np(pthread_self(), &attr) == 0)
            {
                void * stack_addr = nullptr;
                size_t stack_size = 0;

                if (pthread_attr_getstack(&attr, &stack_addr, &stack_size) == 0)
                    __stack_high = (void **)((byte_t *)stack_addr + stack_size);

                pthread_attr_destroy(&attr);
            }
        #elif defined(_WIN32)
            unsigned long long low = 0, high = 0;
            ::GetCurrentThreadStackLimits(&low, &high);
            __stack_high = (void **)high;
        #endif
        }

        return __stack_high;
    }

}   // namespace april_gc
