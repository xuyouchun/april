# Threads

`System.Threading` has real OS threads, but **one managed thread runs at a time**: the one that holds the global lock
(the GIL). The heap, the garbage collector and the tables that are made when they are first needed (the metadata, the
generic instances, the string pool) need no lock of their own, and a collection finds the other threads stopped at places
that are known.

## When the lock goes to another thread

* **A thread waits**: `Thread.Sleep`, `Thread.Join`, `Monitor.Enter` of a lock that another thread holds, `Monitor.Wait`.
* **The turn is asked**: a thread that waited more than 10 ms for the lock sets a flag; the thread that runs reads it at
  the **safepoints** and gives the lock (the threads that wait get it in the order they came). The safepoints are the
  calls and the jumps that go back of the interpreter, and the jumps that go back of the native programs (and the JIT).
  So `while (!flag) { }` ends when another thread sets the flag.
* `Thread.Yield()` gives the turn at once.

A thread that does not wait and does not loop (a long recursion) keeps the lock until it ends: that is a matter of
fairness, not of correctness, as everything that blocks gives the lock.

## The library

| Type | Members |
| --- | --- |
| `Thread` | `Thread(Delegate<Void>)`, `Thread(Delegate<Void, Object>)`, `Start()`, `Start(Object?)`, `Join()`, `Join(Int32)`, `Join(TimeSpan)`, `IsAlive`, `IsBackground`, `Name`, `ManagedThreadId`, `CurrentThread`, `Sleep`, `Yield` |
| `Monitor` | `Enter`, `TryEnter` (with a time), `Exit`, `IsEntered`, `Wait` (with a time), `Pulse`, `PulseAll` |
| `SynchronizationLockException` | `Exit`, `Wait` or `Pulse` by a thread that does not hold the lock |

* `ThreadStart` is `Delegate<Void>`, `ParameterizedThreadStart` is `Delegate<Void, Object>`. A method group that has to
  choose between the two constructors is not resolved by the compiler: put it in a variable of the delegate type first
  (`Delegate<Void, Object> run = Work; new Thread(run)`).
* `lock (obj) { }` is `Monitor.Enter` and `try / finally { Monitor.Exit }` (see [compiler-generated.md](compiler-generated.md)).
* The program ends when the threads that are not in the background (`IsBackground`) end. An exception that nothing
  catches in a thread ends the program (exit code 1), like in the main thread.
* `volatile` is accepted and ignored: a thread switches only at the safepoints, every read sees the last write.

## The runtime

The threads are made by `rtlib::thread_manager_t` (`include/modules/thread_manager.h`, `modules/rtlib/thread_manager.cpp`),
which the interpreter and the native programs share:

* the global lock is a queue (the threads get it in the order they asked for it);
* a thread that blocks is **parked**: it gives the lock and waits for a flag of its own, that another thread sets;
* the lock of an object (`Monitor`) is a table `object address -> monitor`, made of the objects that are held or waited
  for (the objects do not move, and an object that has a lock is reachable from the stack of the thread that uses it);
  `Exit` gives the lock to the thread that has waited the longest, `Pulse` moves a thread of the waiting queue to the entry
  queue, `Wait` gives all the counts of the lock back and takes them again;
* the garbage collector reads the managed objects of the threads that have not ended (`Thread` objects).

The functions of the library (`Thread_Start`, `Thread_Join`, `Monitor_Enter`, ...) are in `modules/rtlib/threads.cpp`.

### The interpreter

A thread has its own `command_execute_context_t` (the stack of execution, the exceptions, the string pool), registered as a
root of the collector when it begins. The thread runs the static method `System.Threading.Thread.__Entry(thread)`
(`__thread_host_t` in `modules/exec/exec.cpp`). The collector scans the native stack of the thread that collects, and the
execution stacks of all the threads: the other threads are stopped between commands, no object is held only by their native
stacks.

### The native programs and the JIT

* The thread function is `Thread.__Entry`, whose address the program gives to `april_native_thread_start`.
* The jumps that go back read a flag (`april_native_yield_flag`) and call `april_native_safepoint`.
* The pending exception of the program is one variable (`@april.exception`): a thread that stops keeps it, and the one that
  runs gets its own back (`thread_switch_hooks_t` in `native_rt.cpp`).
* `native_gc` keeps, for each thread that is stopped, the top of its stack, its registers (`setjmp`) and its pending
  exception, and scans them: **a thread must stop only through `thread_stopping`**, which the manager calls when it gives the
  lock.

## Not done yet

`Interlocked`, `Volatile`, the events (`ManualResetEvent`, `AutoResetEvent`), `SemaphoreSlim`, `[ThreadStatic]`,
`ThreadLocal<T>`, the thread pool and the tasks (phases 2 and 3). `Thread.Abort`, priorities and the affinity are not
planned. The GIL can be taken away (phase 4); the safepoints and the registry of the threads are what it needs.
