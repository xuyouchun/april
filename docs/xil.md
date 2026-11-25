# XIL: tools and optimization

XIL is the instruction set that the compiler writes for every method. The interpreter runs it (each xil is a command), the JIT and the
native back ends make LLVM IR of it. This page tells the tools that show the xils, the passes that change them, and how to measure
and test a pass.

## Seeing the xils

The tools are in `modules/core/xil_tools.cpp`, they are asked by the environment of the compiler (`april compile`, `april run`):

| Variable | What it does |
|---|---|
| `APRIL_DUMP_XIL=<text>` | prints the xils of the methods that have `<text>` in their names (`*` is all of them), with the index of each and the place that a jump goes to |
| `APRIL_XIL_STATS=1` | when the process ends, tells how many xils were written, by kind (the numbers of a xil are one: `push constant (int32)#`) |
| `APRIL_XIL_STATS=2` | the same, and the pairs of xils that follow each other: what a superinstruction can take |
| `APRIL_VERIFY_XIL=1` | checks the structure of the xils of each method (a jump goes into the method), a mistake is an error of the compiler |

Only the code that is compiled is shown: the library `System.apl` is compiled once, to see its xils compile it again
(`april compile core_lib/System/System.project -o .`) with the variable set.

```
> APRIL_DUMP_XIL=Sign april compile prog.cs -o out
== System.Int32 Sign(System.Int32 x)  (14 xils)
  0   push argument int32 [0]
  1   push constant (int32)0
  2   greater int32 int32
  3   jmp false 3  -> 6
  ...
```

The semantic check of the xils is the one of the back ends: the JIT and native generators check the stack of every method strictly
("inconsistent evaluation stack", "jump out of the method"), a program that they reject runs with the interpreter and the test suites
tell it (`test_jit.py` says xfail). So the way to check a pass is to run the three suites with the pass on (below).

## The passes

`xil_optimize` runs after the xils of a method were made and numbered, before the jumps, the switch tables and the protected blocks
take the places of their labels (that is what lets a pass delete a xil: `xil_pool_t::erase_if` numbers the xils again, and the labels,
that are not in the pool, go to the xil that follows them).

They run when the compile controller asks (`compile_optimize_code_t::optimize_xils`, level 1 of `-O`) or when `APRIL_XIL_OPT=1`
is in the environment; `APRIL_NO_XIL_OPT=1` stops them. The passes (repeated until nothing changes):

1. **jump threading**: a jump (always, or when a condition is true or false) that goes to a jump that always goes, goes to the place that
   the second goes to.
2. **unreachable code**: the xils after a xil that ends the flow (a jump that always goes, `ret`, `throw`, `rethrow`) up to the next
   place that something goes to. The places are the labels of the jumps that are left, of the switch tables and of the blocks; the
   marker `end finally` and the last xil of a method are kept.
3. **jump to the next xil**: a jump that always goes, to the xil that follows it, is deleted.

**`-O` and the compile of files.** The legacy optimizations that `-O` names (`compute_constant_values`, `remove_unnecessary_expressions`,
...) are given to the compiler by the compile of *solutions* only: `april compile file.cs` and `april run file.cs` make the compiler with
the default controller, which says no to all of them. Do not "fix" that to see the xil passes: with the legacy ones on, most of the
test programs fail (they were not run for a long time). That is why the passes of the xils have their own variable.

## Measuring

```
python tools/bench.py                      # the programs of bench/: time of the run, xils, output
python tools/bench.py --save bench/baseline.json
python tools/bench.py --xil-opt --compare bench/baseline.json
python tools/xil_corpus.py --verify        # the xils of all the test programs, with and without the passes
```

`bench/` has programs that run for one second or more with the interpreter: loops, recursion, fields, virtual calls, arrays, a generic
list, structs, strings, lambdas. `bench/baseline.json` has the numbers of the compiler without the passes, on the machine that the
file was made on: compare the *change* of a run on a machine, not the milliseconds of two machines.

## Testing a pass

Run the suites with the passes forced on; the library is compiled with them too:

```
APRIL_XIL_OPT=1 APRIL_VERIFY_XIL=1 april compile core_lib/System/System.project -o .
cd test/unittest
APRIL_XIL_OPT=1 APRIL_VERIFY_XIL=1 python -m pytest -q --ignore=test_jit.py --ignore=test_native.py
APRIL_XIL_OPT=1 python -m pytest -q test_jit.py
APRIL_XIL_OPT=1 CLANG=... APRIL_LIBS=... python -m pytest -q test_native.py
```

Then compile the library again without the variable. A pass is right when the three suites give the same results as without it,
and the xil count of `tools/xil_corpus.py` goes down.

## Ideas that are not done

Superinstructions in the interpreter (the pairs of `APRIL_XIL_STATS=2` show the sequences: `push local; push constant; add; pop local`,
compare and jump); inlining of the small methods (getters, indexers, `Nullable.HasValue`); the temporaries that the lowering makes
(`switch`, `with`, exception filters) could share places; dataflow over the locals (copy propagation, dead stores); escape analysis of
the cells of lambdas and of boxed values. Each one needs the tools above first: measure with `bench/`, then check with the suites.
