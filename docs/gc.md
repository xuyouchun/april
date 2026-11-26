# The garbage collector

`new_obj` / `new_array` / the strings that programs make are allocated in a heap that a stop-the-world mark and sweep
collector frees. It does not move objects. The interpreter and the JIT use `modules/mm/gc_heap.cpp` (it knows the types);
the native programs use `modules/native/native_gc.cpp` (it knows nothing of them, it reads the words). Both are made on
**the space of the collector**, `include/modules/gc_space.h` + `modules/mm/gc_space.cpp`, that holds the memory, the marking
and the sweeping and includes nothing of the project. `APRIL_GC=0` switches the interpreter back to the heap that never frees.

## The space (`april_gc::gc_space_t`)

* **The memory is a region** that is reserved at the start (32 GB of address space, or what the system gives; `APRIL_GC_RESERVE`)
  and committed by blocks of 64 KB. The block of an address is `(address - base) >> 16`: finding what a word points to is a
  read of a table (`__kind`), not a search, and a word that is not in the region is rejected by one comparison.
* **A small object** (a cell up to 8 KB, 32 size classes) is in a block of its size class **and of its kind**: the blocks of
  *atomic* cells hold objects that have no references (the characters of a string, the arrays of numbers, the objects that
  have no reference fields). Such an object is marked and never scanned, and not even pushed on the mark stack.
* A block has a byte map, an entry per granule of 16 bytes: bit 7 is the mark, bits 0-6 are `0` for a cell that is free, else
  `1 + the extern area / 8` (the type word and the lengths of an array, before the object). The index of the cell of a word is a
  multiplication by a reciprocal (the division was the cost of a mark).
* **A large object** (more than 8 KB) has a run of blocks and a header; only the pages that it needs are committed, and
  they are 0 (a new array is not cleared). The words that point into the run are found by the table (`__aux`: the distance to
  the head), so a pointer into the middle of an array is as cheap as the others.
* **The address of an object is inside its cell** (`cell + extern`, and a cell has at least a byte after the extern area): the
  reference to an empty array does not point to the next cell.
* **Allocation** is a cursor in the block (`block_t::cursor`): the next entry of the byte map that is free; a block that was
  committed now is 0 (`zero_from`), the others are cleared cell by cell. The sweep reads the byte maps and counts, it does not
  touch the cells: **a block in which nothing is marked is given back whole** (a few stay committed, `__warm_blocks`; the rest
  goes back to the system), the others keep the cells that are free for the cursor. A collection starts when the bytes
  allocated since the last one reach the threshold (16 MB, then twice the live bytes: `APRIL_GC_THRESHOLD`, `APRIL_GC_GROWTH`).
* **Weak references** (`weak_new/get/free`): a table of hidden addresses that the marking does not read; after the marking the
  ones whose object was not marked are cleared. **Finalizers**: an object that has a destructor is registered when it is made
  (`gc_scan_info_t::finalizable`); when it is not marked it is queued and marked (it is alive, and what it holds, until the
  queue gives it: `take_finalizable`), and it is freed by the collection that follows.

## The heap of the interpreter (`gc_heap_t`)

* The roots are the execution stacks of the contexts (conservative, minus the value-only region of the locals of every frame,
  `gc_value_begin/end`), the exception chain, the static data and the thread statics (precise), the native stack and the registers.
* The words of an object are read by its **scan info** (`gc_scan_info_t`, resolved from the metadata by
  `modules/exec/gc_resolver.cpp`, cached by type: a table by the address in front of the map): `none` (the object is atomic),
  the byte ranges of its reference fields, or all of it. A type that cannot be resolved is read all (an array of them: by steps of 4).
  A class that has a destructor, or a base class that has one, is `finalizable`.
* A **string is a reference** for the collector: `__size_of_struct` gives the storage type `ref` to the `string_` vtype, so a
  class field, an array element or a struct that has a string is scanned (the struct is a `mixture`). In the locals of a method
  the strings stay in the group of the values (that layout is arranged by the index of the variables of the blocks), but they are
  flagged (`__item_t::holds_ref`) and left out of the value-only region.
* **The strings of the library** (`rtlib::set_string_allocator`) are atomic spaces of the heap (`rt_heap_t::new_atomic`); a
  reference points to the characters, 8 bytes after the head, which is inside the cell. They were never freed before, and did not
  count for the threshold.

## What the collector needs from the layouts (found by `basic/gc_*.cs`)

1. A reference must be where a scan finds it. The frames, the objects and the blocks are read by words of 8 bytes, so the
   variables of a frame are aligned to their size (`locals_layout_t::__set_offsets` put a variable at an unaligned offset when a gap
   was filled before it: a string at the offset 29 was lost).
2. The elements of an array of structs are as large as the struct: a struct with a reference and an `Int32` is 12 bytes, so the
   reference of the second element is at the offset 12. These arrays are read **by steps of 4** (`gc_scan_info_t::unaligned`), and so are
   the objects of the native collector.
3. The address of an object is inside its cell (see above): `new UInt8[0]` was freed while a field pointed to it.

## System.GC, WeakReference, destructors

* `System.GC`: `Collect`, `CollectionCount`, `GetTotalMemory(bool)`, `MaxGeneration` (0), `GetGeneration`, `KeepAlive`,
  `SuppressFinalize`, `ReRegisterForFinalize`, `WaitForPendingFinalizers`. `WeakReference` and `WeakReference<T>` (`TryGetTarget`,
  `SetTarget`; their destructor gives the handle back). The library functions are `GC_*` in `modules/rtlib/gc.cpp`; the back end gives
  them the services (`rtlib::set_gc_services`).
* **A destructor** (`~C()`) runs after the collection that found the object, **at the next safepoint** of the thread that runs
  (a call, a jump back: the collector sets the flag of the yield, `rtlib::set_safepoint_hook`) in a context of its own, the destructor
  of the class first, then the ones of its base classes. What a destructor throws is lost. They do not run when the program ends. Limits:
  the classes that are generic are not finalizable (the resolver does not resolve them); a **native program does not run destructors**
  (`GC.SuppressFinalize` is not supported there: a program that uses it is run by the interpreter; the other `GC` functions and the
  weak references work).
* The collector looks at the stacks without knowing what a word is: an object can stay alive because of a stale word (one of twenty
  objects that were made and dropped, in `basic/gc_api.cs`), so a program must not count on a destructor to run at a given time.

## Tools

| Variable | |
|---|---|
| `APRIL_GC=0` | the old heap, nothing is freed |
| `APRIL_GC_TRACE=1` | a line per collection: collected, live, bytes scanned / skipped, the threshold, the times of mark and sweep |
| `APRIL_GC_STATS=1` | a summary when the heap ends: collections, total / max pause, mark / sweep (a native program: `collections=`, `peak=`) |
| `APRIL_GC_THRESHOLD=n[K\|M]` | bytes allocated before a collection (and the floor of the threshold), `APRIL_GC_GROWTH=f` the factor of the live bytes, `APRIL_GC_RESERVE=n[M\|G]` the address space |
| `APRIL_GC_STRESS=1` | collects at every allocation |
| `APRIL_GC_POISON=1` | fills the freed cells with `0xDD`: an object freed by mistake fails at once |
| `APRIL_GC_VERIFY=1` | reports the words of the regions that a scan info skips and that point to the heap. **Noisy now**: the value regions of structs that have constants (`TimeSpan` is 48 bytes, the constants have a place) hold the stale words of the stack, and the strings are in the heap, so the reports are not a bug (`regex_*` tests); use it on small programs |

The regression that checks the collector: the suites with `APRIL_GC_THRESHOLD=64K APRIL_GC_POISON=1`, with `APRIL_SUPER=eager
APRIL_OSR_THRESHOLD=3 APRIL_GC_THRESHOLD=32K`, and `test_jit.py` (it runs every program also with `APRIL_GC_STRESS=1`);
`test/gc_standalone` (`run.sh`) tests the space and the heap without the rest of the toolchain (476 checks).
Tests: `basic/gc_strings.cs`, `basic/gc_roots.cs`, `basic/gc_api.cs`; benchmarks: `bench/gc_garbage.cs`, `gc_live.cs`, `gc_arrays.cs`, `gc_strings.cs`.

## Measures (interpreter, `bench/gc_*.cs`; before = the heap of blocks of 64 KB with a hash set and a free list per block)

| | no GC | before | now |
|---|---|---|---|
| 6 million short lived objects | 0.62 s, 201 MB | 0.55 s, 35 MB; 11 collections, 65 ms, sweep | **0.36 s**, 34 MB; **0.4 ms** (a block with nothing marked is given back whole) |
| a live list of 1.5 million nodes + garbage | 0.68 s, 188 MB | 0.60 s; mark 147 ms, max pause 83 ms | **0.43 s**; mark 71 ms, max pause 39 ms |
| arrays that die at once | 0.47 s, 545 MB | 0.47 s, 32 MB; 54 ms | **0.18 s**, 18 MB; 5 ms (atomic arrays, no clearing) |
| 600 thousand strings | 0.52 s, 92 MB | 0.41 s, 76 MB, never collected | **0.28 s**, 35 MB |
| 6 million strings | 2.14 s, 770 MB | 2.0 s, 758 MB, never collected | **1.37 s**, 35 MB; 1.4 ms |

What is left in the list of 1.5 million nodes is the chain itself: a mark follows one pointer after the other, each one a miss of the
cache (27 ns an object). The JIT suite runs in 9 minutes, it was 15 with the native collector of a `std::map`.

## Not done, and why

* **Generations / moving**: a young generation needs a write barrier in every command that stores a reference (the interpreter has many
  of them, with the superinstructions), in the JIT and in the native programs, and the conservative stacks pin what they point to. The
  pauses that are left are the marks of the live set, that a young generation does not reduce when the garbage is already free (the
  first three rows above).
* **A parallel mark**: the chain of a list cannot be shared, and the type resolver and the caches are not made for threads.
* The constants of a struct do not need a place in it (`TimeSpan` has 48 bytes, 8 are the value): the layout of the types (and the
  JIT and native back ends that read it) would change, it is not a matter of the collector.
* The finalizers of the generic classes, and of the native programs.
