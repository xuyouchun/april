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
| `Thread` | `Thread(Delegate<Void>)`, `Thread(Delegate<Void, Object>)`, `Start()`, `Start(Object?)`, `Join()`, `Join(Int32)`, `Join(TimeSpan)`, `IsAlive`, `IsBackground`, `Name`, `ManagedThreadId`, `CurrentThread`, `Sleep`, `Yield`, `SpinWait`, `MemoryBarrier` |
| `Monitor` | `Enter`, `TryEnter` (with a time), `Exit`, `IsEntered`, `Wait` (with a time), `Pulse`, `PulseAll` |
| `Interlocked`, `Volatile` | `Increment`, `Decrement`, `Add`, `Exchange`, `CompareExchange` (`Int32`, `Int64`, references), `Read`; `Volatile.Read/Write<T>` |
| `WaitHandle` | `WaitOne` (with a time), `WaitAll`, `WaitAny`, `WaitTimeout` |
| `ManualResetEvent`, `AutoResetEvent`, `Semaphore`, `Mutex` | wait handles (`Set/Reset`, `Release`, `ReleaseMutex`); the mutex is of the program and can be taken again |
| `SemaphoreSlim`, `ManualResetEventSlim`, `CountdownEvent`, `Barrier` | the light ones, each with the lock of its own object |
| `ReaderWriterLockSlim` | `Enter/Exit` + `TryEnter` of the read and of the write lock (no upgradeable lock, no recursion) |
| `SpinWait`, `SpinLock` | `SpinOnce`, `SpinUntil`; `Enter(ref Boolean)`, `TryEnter`, `Exit` |
| `ThreadLocal<T>`, `[ThreadStatic]` | `Value`, `IsValueCreated`, a factory; the attribute `System.ThreadStaticAttribute` on a static field |
| `CancellationTokenSource`, `CancellationToken`, `CancellationTokenRegistration` | `Cancel`, `Token`, `Register`, `ThrowIfCancellationRequested`, `CreateLinkedTokenSource` |
| `Lazy<T>` (namespace `System`) | `Value`, `IsValueCreated`, a factory, thread safe or not |
| `Timeout` | `Infinite`, `InfiniteTimeSpan` |
| exceptions | `SynchronizationLockException`, `SemaphoreFullException`; `OperationCanceledException`, `ObjectDisposedException`, `TimeoutException` (`System`) |

* The handles of `WaitHandle` share one lock (`typeof(WaitHandle)`, the gate): a change wakes all the threads that wait, each
  checks its own condition. That is what lets one thread wait for many handles. The light types (`...Slim`, `CountdownEvent`,
  `Barrier`, `ReaderWriterLockSlim`) use `Monitor.Wait` / `PulseAll` on themselves.
* `Interlocked` on numbers is plain code: a thread switches at a call that waits or at a jump that goes back, and the body has
  neither. The ones of references take the lock of the class.
* The static constructors of the libraries (a static field with an initial value) run when the program starts, before the
  entry point, the core library first (`each_static_constructor` of the assembly, in the interpreter and in the native programs).

* `ThreadStart` is `Delegate<Void>`, `ParameterizedThreadStart` is `Delegate<Void, Object>`: `new Thread(Work)` picks the
  constructor by the parameters of the method.
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

## `[ThreadStatic]`

A static field that has `[ThreadStatic]` is kept in an object of the thread: the compiler pushes the object by
`push object thread_static_data` instead of `static_data`. The interpreter makes the object of a (thread, type) when it is first
asked (`executor_env_t::thread_static_data_of`), a program that is native does it by `april_native_thread_static`; both are roots
of the collector. The object has the layout of the static data of the type, only the fields that have the attribute are used
of it. As in .NET, the initial value of the field is for the thread that runs the static constructor; a thread that begins reads
the default of the type. The objects of the threads that ended are not freed (a thread is small).

## Not done yet

The upgradeable `ReaderWriterLockSlim`, `CancelAfter` and the timers, the thread pool and the tasks (phase 3). `Thread.Abort`,
priorities and the affinity are not planned. The GIL can be taken away (phase 4); the safepoints and the registry of the threads
are what it needs. A condition expression `c? a : b` whose value is a struct of a generic param and that has a call as a branch
is not supported.
