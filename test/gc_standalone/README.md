# GC standalone tests

Behavioral tests of the garbage collector that build and run **without boost and
without the rest of the toolchain**, under ASAN/UBSAN:

- `gc_test.cpp`: `modules/mm/gc_heap.cpp` against a minimal shim of the runtime
  headers (`shim/`). Covers allocation & object/array layout, conservative stack
  roots, interior pointers, payload tracing, precise roots, 1-D / multi-dimensional
  arrays, large objects (>8KB), stress mode, manual alloc/free semantics, and the
  precise scan infos of Phase 2a (`none` / `ranges` kinds, resolver failures,
  range clipping, verify mode, a linked list under stress + verify).
- `gc_stack_test.cpp`: the frame walker `modules/exec/__gc_stack.h` of Phase 2b
  (value-only regions of locals skipped, and every inconsistent chain falling
  back to scanning the whole stack).

Run:

```sh
./run.sh
```

Note: the shim must be kept in sync when the real `rt_heap_t` / `gc_root_source_t` /
`gc_scan_info_t` interfaces change.

## Debugging a real run

| Variable               | Effect                                                              |
|------------------------|---------------------------------------------------------------------|
| `APRIL_GC=0`           | Uses the old never-freeing heap.                                    |
| `APRIL_GC_STRESS=1`    | Collects on every allocation: exposes missing roots.                |
| `APRIL_GC_VERIFY=1`    | Reports words in regions the precise scan skipped that point to gc objects: exposes wrong scan infos (look for `[gc-verify]` on stderr). Slow. |
| `APRIL_GC_TRACE=1`     | One line per collection: bytes collected / live / scanned / skipped. |

Suggested check on a real build: run the pytest suite with
`APRIL_GC_STRESS=1 APRIL_GC_VERIFY=1`; any `[gc-verify]` line or crash is a bug in
a scan info or a missing root.
