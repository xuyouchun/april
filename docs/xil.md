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
(`switch`, `with`, exception filters) could share places; dataflow over the locals (copy propagation, dead stores: measured on the xils
of 120 programs (29438 xils, 1170 methods), a `pop local x` followed by a `push local x` is 1.5% of them, a local that is written and never
read 0.2%, a local that is written once and read once 1% of the locals: not worth a pass); escape analysis of
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
| array_load | `[push index] [push array] push array_element`: one dimension, any type of element (numbers, boolean, char, objects, strings). The index is a local, an argument or a constant, the array a local or an argument; one or both can be the stack |
| array_store | `[push x] [push index] [push array] pop array_element [jmp]`: as above, the value x can be a constant too; the jump is the one that ends a loop |
| ret_push | `push x, ret` (a method that returns one unit) |

The binary form has one more tail: the result is returned (`[x] [y] OP ret`), with the same operations and compares as above. The array
commands raise the null reference and the out of range exceptions as the commands of the xils do (the same index of command, the same
blocks). The size of the frame of `ret` is in the command, a method that returns a struct or nothing keeps its commands.

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

**Measured when the arrays and `ret` were added** (`eager`, 5 rounds): `array_sieve` 1.25x, `field_access` and `string_build` 1.1x (noise
or arrays of characters), `fib` and `virtual_calls` 1.05x from `ret`; the others are inside the noise of the measure (about 4%).

Ideas not done: the commands of the calls (`push x ... call`): the call and the return are most of the cost of a call, and joining the
pushes saves one dispatch of about four; the fields of other objects than this (the check of null is the part to keep).

## The accessors of the properties (`analyze_trivial_method`)

A method that only reads a field of this (`push argument this, push field, ret`) or writes one (`push argument value, push argument
this, pop field, ret`) is a getter or a setter: the command of a call of it (`call instance`, a non virtual call, of a class or a struct,
generic or not) does the access itself and makes no frame (`__trivial_call` in `commands.cpp`; the frame, the safepoint of the call and
the return are not needed). The parser finds out what a method is when it makes its commands (`exec_method_t::trivial_kind`, the type
and the offset of the field), so the first call of a method is a real call and the next ones are not; the null object of a class is
still checked by the command of the call. A property that does more than the access, a virtual or an interface method, a method that
returns a struct are real calls. `APRIL_INLINE_ACCESSORS=0` turns it off. `bench/accessors.cs` (properties in a loop of 20 million
rounds): 1.6x without the superinstructions and 1.3x with them; the other benchmarks do not use accessors much (inside the noise).
`test/unittest/basic/inline_accessors.cs` has every type of field, a struct, a generic class, a property that does more, an override,
the null object, and the calls that come before and after the method is parsed.

A method that does nothing (`ret` only: the constructor of Object) is the third kind: the call pops this and goes on, and the constructor
that only calls the one of the base class (`push this, call, ret`) is the same when the one that it calls does nothing (it is found when
its commands are made, the called method is made first). The chain of constructors of a class that has no constructor of its own costs
no call then (`new C()` with C : B : A : Object was four calls). `test/unittest/basic/empty_constructors.cs`. `struct_math` and
`string_build` 1.1x.

## The inlining of calls (`__expand_calls` in `modules/exec/parser.cpp`)

The body of a small method is put in the xils of its caller when the commands of the caller are made. It works in the interpreter, for
every assembly: the callee is read with its own parser and its own context (`exec_method_t::new_inline_parser`, a parser that does not
change the method), and the commands of its xils are made with that context, so its refs (types, methods, fields, strings of the
library) are those of its assembly: there is nothing to translate between assemblies. The JIT and the native back end have the
inlining of LLVM.

**What is inlined** (`inline_check_body`): a static method or a non virtual instance method (of a class, a struct, a generic one) that
has at most 24 xils (`APRIL_INLINE_SIZE`), no protected block, no switch, a `ret` as the last xil (others may be in the middle), params of
one unit each (no `ref`, `out`, `params`), no struct result, and only the xils that need no frame of their own (the calls of internal
functions, `copy`, `init`, `pick`, the stack allocations are not here: a `copy` of a generic non struct value is allowed). The forward jumps, `if`, `throw` are in; a loop (a backward jump) is not. The accessors and the empty methods are not inlined (they have their own fast call).
A method that calls itself is not inlined; the calls in a body stay calls.

**How**: the variables of the callee get a place in the frame of the caller, after its own variables (`locals_layout_t::reserve`,
`set_shift`: the layout of the callee is the same, shifted), and its params get places too (`add_extra`: a variable of the layout that is
the place of a param). The xils that push or pop an argument are made again as the ones of a local variable (`inline_rewrite_xil`), and
the last `ret` is not copied (the value is at the top of the stack as the call leaves it), a `ret` in the middle of the body is a jump to
the end of it (`inline_rewrite_xil`; the other jumps of the body keep their steps: the xils of a body are one after the other). The first command of a body
(`__inline_enter_command_t`) moves the args of the stack to their places, initializes the type of a static method and checks the
object of a call of a class for null, as the command of the call did. The jumps, the switch tables and the blocks of the caller go to
the new places (a map of the xils; the steps of the jumps that were changed are made again). The frame is bigger: the first call of a
method makes its commands before its frame (`push_calling` asks for the commands first), and its `stack_unit_size` is the new size.

**The args that are not pushed**: the last args that the caller pushed just before the call (a local variable that has no address
taken, the address of a local variable, a number or a boolean) are not pushed, the body reads the variable itself: `Add(i, 1)` is
`push i, push 1, add` as if it was written there, and the fusion joins it. A string or an object constant is not forwarded (its ref is
the one of the caller, the body is made with the context of the callee: the result was a wrong string). The object of a call of a class
is never forwarded (it must be checked), nor an arg that the body changes. Nothing that is a place that a jump goes to is dropped but
the first of the pushes.

The commands of the different bodies are not joined (the plan has a cut at the begin and at the end of each one), and the plan is
not shared by the instances of a generic method. `APRIL_INLINE_CALLS=0` turns the inlining off, `APRIL_INLINE_LIMIT=n` inlines the first
n calls only (to find the one that is wrong: the result changes at the n that is the first bad one), `APRIL_INLINE_TRACE=1` tells
each one. `APRIL_CALL_STATS=1` tells at the end of the process how many calls there were, by kind, and by the size of the callee.

**What it gives**: the loop of two calls of small static methods (`call_cost`, 30 million rounds): 1.2x (the same code written inline
is 1.33x). In the benchmarks, 1.0x - 1.07x (`field_access` 1.07x, `struct_math` 1.04x, the others inside the noise): the calls that
they have are to accessors (a fast call already), to methods that have a jump (the check of the index of `List`), to the virtual and
the interface methods, to the invoke of delegates, and recursive ones, which are not inlined. A method with an `if` is the next step
(its jumps must be made again, the `ret` in the middle becomes a jump).

**What was wrong, and is in the tests**: the inlining made `field_access` 14% slower first: the inlined `this` is a local variable,
and the superinstructions of `this.Field` (the read and the write of a field of this) were for an argument only. They take a local
variable that is an object now, which makes the reads and writes of the fields of any object in a local variable faster too.
`test/unittest/basic/inline_calls.cs` has static and instance methods, a struct, a generic method, args that the callee changes, its
variables, the calls inside it, an exception that it raises (the caller catches it), the null object, a static constructor, and
callers with loops, a switch, try blocks and many calls.

A method with an `if` (`Max`, `Abs`, `Clamp`, a loop) is inlined: `test/unittest/basic/inline_branches.cs`. The benchmarks do not have many
(the result is inside the noise: `fib` is recursive; `List<T>.get_Item` is inlined now, `generic_list` 1.08x).

## Virtual calls, interface calls and delegates (OSR: replacing the commands of a method that runs)

The method that a virtual or interface call goes to, or a delegate calls, depends on the object. A body can be put in the xils only after
a check of the type of the object, with the call kept for the objects of other types, and the type must come from a profile. The first parse
of a method is the only one that was made before, so the profile could not reach a loop in a method that is called once (`Main`). It is
done now by the replacing of the commands of a method while it runs (on-stack replacement, VIRTUAL model only; the INLINE model has stubs).

**The profile**: when a method has a loop and a virtual call, an interface call or a call of a delegate (`Invoke`) in it, those commands are
made as their own copies (`command_creating_context_t::osr_candidate`: the commands are shared by key, the profile must not be) and count
what they saw (`exec_call_profile_t`: the type of the first object, its method, hits, misses). The frame of the method has 40 spare units
(`__osr_spare_units`), the first version of the commands does not use them.

**The replacing**: a call that went 2000 times (`APRIL_OSR_THRESHOLD`) to one type, with at most 5% of other ones (`exec_osr_qualifies`)
makes the method parse again (`exec_method_t::osr_upgrade`, `method_parser_t::parse_osr`). The second version is made by the same
`__expand_calls`: the call is replaced by *check of the type, the body of the method, a jump over the call, the call* (the check is
`__osr_guard_command_t`: a jump that goes to the call when the object is null or of another type; for a delegate
`__osr_delegate_guard_command_t` checks the method that the delegate has). The commands of both versions have the same frame (the
variables of the bodies use the spare units), the xil indexes of the first version map to the second (`exec_osr_t::command_of_xil`), and
the loop switches to the new commands at the next backward jump (`exec_osr_t` keeps the blocks of both versions, `find_block` looks in
both). The upgrade is done once: when it fails or finds nothing, the profiles are detached (`exec_osr_detach`) and the calls do not count
any more (a call site with many types, as `virtual_calls`, has no cost after that).

**Not done**: a method without a loop (it is made again when it is called the next time with a better profile: not done), a receiver that
is a value type or a string, more than one upgrade, the INLINE and MANUAL models. A delegate is a struct, so there is no null delegate to
guard.

**Others**:

* **The cache of an interface call**: the call looked for the function of the interface in the table of the type of the object each
  time. The command has a cache now of up to four types and the methods that they have (`__call_site_cache_t`, a cache is made again
  when a type is added, it is never changed: other threads read it; value types and strings keep the search, they need the fix of the
  receiver). `virtual_calls` (a site that has two types): 1.4x.
* **Invoke of a delegate**: a call of `Invoke` (`call command delegate_invoke; ret`) is recognized as the fourth kind of method that
  `analyze_trivial_method` finds (kind 4): the call goes to the method that the delegate has, without the frame of Invoke
  (`__delegate_invoke_direct`). `test/unittest/basic/delegate_direct.cs`.
* **A method that returns through a generic `copy`** (`List<T>.get_Item`) is inlined (a `copy` of a non struct type and `object_copy` are
  allowed xils). A body with a loop is not inlined (a loop in a callee hid the sites of the OSR in the loop of the caller).

**What it gives**: a loop with a monomorphic interface or virtual call (`bench/monomorphic.cs`) 1.33x - 1.46x, a loop that calls a delegate
(`bench/delegate_loop.cs`) 1.25x, `lambda_loop` 1.08x, `generic_list` 1.08x (the inlined `get_Item`), `virtual_calls` (two types: the
upgrade is not done) 1.0x. Tests: `test/unittest/basic/osr_calls.cs` and `osr_delegates.cs` (the output is the same with `APRIL_OSR=0`);
run the suites also with `APRIL_OSR_THRESHOLD=3` to make the replacing happen in many tests (done: eager, auto+1 and default pass).
`APRIL_OSR=0` turns it off, `APRIL_OSR_TRACE=1` tells each upgrade.

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
the delegates (`<` missing), of the commands of the reflection and of the profile; the superinstructions have values from 7800
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

**Speeding up INLINE** (the disassembly of `__execute_commands` with `llvm-objdump`, then the same programs; the noise of the
measure is about 4%):

* *The values of the commands in order*: they are in order already (`__COUNTER__`: 0 - 7790, 99.9% used). The gap was the
  superinstructions at 20000: the table of the `switch` had 21003 entries (84 KB), 8138 used. `__SuperBase` is 7800 now (the
  `static_assert` at the end keeps it above the other values). No change in the time (+2% / -4%, the noise); the library is smaller.
* *The rare and big commands out of the loop* (done): the calls, casts, throw, new, boxes, reflection, `tier`/`profile`/sequence
  cases are `__CaseC` in the lists; the loop calls them through `__execute_cold` (noinline), the other models take them as a plain
  `__Case`. The spills of the loop went away: the references to the slots of the stack of the function fell from about 7000 to 16 and
  the pointers to the stack of the execution, the table of the switch, are in registers. 5% faster without superinstructions, 3% with
  them. Now INLINE / VIRTUAL is about 1.0x without them and 1.04x - 1.08x with them (three programs, `field_access`, `struct_math`, are
  still slower than VIRTUAL).
* *A threaded loop* (tried, not kept): each case ends with its own `goto *table[cmd_value]` (the table of the addresses of the
  labels is filled by a first call of the function). The result was inside the noise (3% faster without superinstructions, 2%
  slower with them) and the code had labels made with `__COUNTER__`: it was taken back.
* *PGO* (the library built with the profile of the benchmarks and of a third of the tests; `-DAPRIL_EXEC_PGO=gen` makes a library
  that writes `LLVM_PROFILE_FILE`, `llvm-profdata merge`, then `-DAPRIL_EXEC_PGO=<file>.profdata`): VIRTUAL, the 9+1 programs of `bench/`: 1.12x
  without the superinstructions (`virtual_calls` 1.44x, `array_sieve` 1.21x), 0.98x with them (`auto`, the default mode): not worth a
  build step. INLINE (one big function) may profit more; it was not tried.
* *Not done*: the pointer of the current command and the top of the stack in local variables. The commands that raise a managed
  exception change `ctx.current` (the handler of the catch) and return, so the local one must be read again after each command
  that can raise: the check costs about what it saves. PGO, and the commands of arrays/calls/`ret` as superinstructions, are the
  other ideas.
