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

## Profile of the commands (APRIL_EXEC_PROFILE=1)

The interpreter runs one command for each xil (`modules/exec/parser.cpp` makes them when a method is called the first time; the commands
are called through a virtual function, `EXEC_EXECUTE_MODEL_VIRTUAL`). To choose the superinstructions (several xils that are one command)
the commands can be counted: with `APRIL_EXEC_PROFILE=1` every command is wrapped by one that counts its executions, and when the
process ends a report is written to stderr (the programs run slower; do not time them). `python tools/exec_profile.py bench|tests` runs
the programs and sums the reports.

The report tells:

* the methods by the number of calls (`>= N`) and the share of the commands that they execute, the methods that have a loop, and the
  share of the rule "a method that has a loop, or that was called N times";
* the time that the parse of the commands takes, and of that the time to find the places that a command must begin at (the targets of
  the jumps and the switch tables, the limits of the try blocks): `exec_cut_points`;
* the xils that are executed the most, and the sequences of 2 to 4 xils by the dispatches that one command for each would save;
* a simulation: the superinstructions that were thought of, joined greedily (the longest first) by a matcher, and the dispatches that
  they save, with the cut places and without them.

What it showed (the 9 programs of `bench/`, and the 357 test programs):

* the dispatches saved by the simulated set: 17% - 53% for the programs of `bench/` (loops of numbers 50%, calls 17%), 42% for the
  test programs; the sequences are those of `local op const`, `local op const -> local`, the compare of a local with a constant and
  its jump, `this.field`;
* the cut places hardly change that (below 0.1% in total): a compact array of commands, where a command cannot have such a place in it,
  loses almost nothing;
* parse of the commands: 12 us for a method, the cut places 0.12 us of it (1%): finding the plan is not what costs;
* the methods that have a loop are 9.5% of the methods and execute 84% of the commands; 63% of all the commands are in methods that
  have a loop and were entered less than 20 times: a count of calls alone would not find them, the rule "has a loop, or was called N
  times" covers all of the commands with 16% - 33% of the methods (N = 100 .. 10).

## Superinstructions (modules/exec/fusion.h)

A superinstruction is **not an xil**: the compiler, the assembly, the JIT and the native back end do not know it. It is a command of the
interpreter that does the work of several xils that follow each other, and it is made when the commands of a method are made
(`method_parser_t::__parse_normal_commands_`).

**The array of the commands is shorter than the array of the xils.** A plan (`fusion_plan_t`) tells the groups of xils and the index of
the command of each xil; the jumps (their steps), the switch tables and the blocks (try, catch, finally) are numbered again with it. A
command cannot have a place in it that a jump, a switch or a block goes to, except at its first xil (`exec_cut_points`: the profile
showed that this costs nothing, below 0.1% of the saving). The plan depends on the xils only, so it is made once for a method and the
instances of a generic method share it (a cache keyed by the xils).

**What is joined** (the xils of numbers: Int32, UInt32, Int64, UInt64, Single, Double, and an Int64 with an Int32):

| Form | xils |
|---|---|
| binary | `[x] [y] OP \| CMP` and a tail: nothing (the result is pushed), `pop local`, `pop local` + `jmp`, or (a compare) `jmp true/false`. The operands x and y are a local, an argument or a constant; one or both can be the stack. OP: add, sub, mul, div, mod, and, or, xor, shifts. |
| push2 | `push x, push y` |
| copy | `push x, pop local` |
| convert | `push x, convert` |
| this_field | `push argument (object), push field` |
| store_field | `[push x] push argument (object), pop field` |

The division raises the exceptions of the command of the operation (DivideByZero, Overflow) and an exception of a command is told with
the index of that command, so the blocks find it. A jump that goes back gives the other threads their turn, as the jump command does.
A group that has a variable that is not of the type of its xil (it is not known when the plan is made) is a command that runs the
commands of its xils one after the other (`__sc_sequence_t`): the plan does not change.

**When the commands are made** (`APRIL_SUPER=off|auto|eager`, `auto` is the default; `APRIL_SUPER_THRESHOLD`, 50):

* `eager`: every method that has a group gets its superinstructions when its commands are made (the test suites run with it);
* `auto`: a method that has a loop gets them at once (a call of it can run for long, and the profile found 84% of the commands there),
  a method that has none and no try block starts with a command that counts its calls and gets them at the threshold
  (`exec_method_t::tier_up`: the parser is made again, the commands are made with the plan and the method uses them from the next call;
  the frames that run the old commands go on with them, they have the same locals); a method that has a try block and no loop keeps
  its commands;
* `off`: the commands of the xils, one for each.

With `APRIL_EXEC_PROFILE` on there are no superinstructions (the profile counts the commands of the xils).

**What it gives** (`tools/bench.py`, the programs of `bench/`, the time of a run, about 250 ms of it is the start of the process): 1.2x to
2.2x with `auto` against `off`; `loop_arith` 2.1x, `array_sieve` 1.8x, `fib` 1.4x, `field_access` 1.5x, `virtual_calls` 1.5x,
`generic_list` 1.3x, `struct_math` 1.3x, `string_build` and `lambda_loop` 1.2x (the calls and the strings are not joined).

**How it was tested**: the test suites with `APRIL_SUPER=eager`, with `auto` and `APRIL_SUPER_THRESHOLD=1` (every method gets them at its
first call, while the other threads call it), and with the passes of the xils on; `test/unittest/basic/superinstructions.cs` has every
form, the types, the jumps into a loop, the division and its exceptions, a loop in a try block, the threads, and the methods that get
their commands after the threshold. The time is not a test, a change of it must be seen with `tools/bench.py`.

Ideas not done: the commands of arrays (`push array_element`, `pop array_element`), of the calls (`push x ... call`), `ret` after an
operation, the fields of other objects than this (the check of null is the part to keep).

## The models of the interpreter (EXEC_EXECUTE_MODEL)

`include/modules/exec.h` chooses how the interpreter calls a command (the default is VIRTUAL; a build can set `-DEXEC_EXECUTE_MODEL=3`):

| Model | Call of a command | A new class of command |
|---|---|---|
| `VIRTUAL` (1, default) | the virtual function `execute` | nothing to do |
| `MANUAL` (2) | a pointer to a function in the command | nothing to do. It is not kept up: not built, not tested |
| `INLINE` (3) | a `switch` on the value of the command (`cmd_value`), the commands are inline in it | every class has a value of its own and a `__Case` in `__BeginSwitchCommands` |

INLINE was not kept up and could not be built; it is complete now and passes the test suites (512 tests with the superinstructions off,
`eager`, and `auto` with a threshold of 1). What was missing: the cases of the pointers (`ptr_content`), of the boxes, of `init`, of the
array element addresses, of the casts to an interface (two classes had one value: the cast to an interface has its own values now), of
the delegates (`<` missing), of the commands of the reflection and of the profile; the superinstructions have values from 20000
(`__SuperBase`, checked by a `static_assert` at the end of `commands.cpp`) and are in the switch (`__SwitchCommands_Supers`). A terminal
exception can go out of a command now (the commands are not `noexcept`), the loop catches it as the other models do.

`APRIL_CHECK_COMMANDS=1` (INLINE only) tells at run time a class of command that is made and has no case, or whose value belongs to
another class (a mistake that runs the wrong class without a message): it found two mistakes of the old code, a pop of the address of an
argument that had the value of a pop of an argument, and the calls of a dynamic method that were registered with the type of the other
methods.

**Speed** (the 9 programs of `bench/`, the time without the start of the process, 9 rounds, the same compiler flags):

| | VIRTUAL | INLINE | INLINE / VIRTUAL |
|---|---|---|---|
| commands of the xils (`APRIL_SUPER=off`) | 13051 ms | 14042 ms | 0.93x (7% slower) |
| with superinstructions (`eager`) | 8668 ms | 8357 ms | 1.04x (4% faster) |

By program: without superinstructions INLINE is slower in all of them (0.90x - 0.99x); with them it is faster in loops of numbers
(`array_sieve` 1.19x, `fib` 1.11x) and slower in the programs of strings and structs (`string_build` 0.95x, `struct_math` 0.97x). The
code of the loop of INLINE is one function with all the commands in it (the library `april_exec` is 12 MB, VIRTUAL has 15 MB): the
register allocation of a function of that size is worse, and the jump of a `switch` is as dear as the call of a virtual function. It
is not worth to change the default; the superinstructions give much more (1.4x - 2.1x) than the model.
