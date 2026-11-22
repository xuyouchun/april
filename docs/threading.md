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
| `ReaderWriterLockSlim` | `Enter/Exit` + `TryEnter` of the read, the write and the upgradeable read lock (one thread holds the upgradeable lock; it reads with the others and goes on to write, `ExitWriteLock` goes back to it; no recursion) |
| `SpinWait`, `SpinLock` | `SpinOnce`, `SpinUntil`; `Enter(ref Boolean)`, `TryEnter`, `Exit` |
| `ThreadLocal<T>`, `[ThreadStatic]` | `Value`, `IsValueCreated`, a factory; the attribute `System.ThreadStaticAttribute` on a static field |
| `CancellationTokenSource`, `CancellationToken`, `CancellationTokenRegistration` | `Cancel`, `Token`, `Register`, `ThrowIfCancellationRequested`, `CreateLinkedTokenSource` |
| `Lazy<T>` (namespace `System`) | `Value`, `IsValueCreated`, a factory, thread safe or not |
| `ThreadPool` | `QueueUserWorkItem` (with a state, without), `ThreadCount`, `PendingWorkItemCount`, `CompletedWorkItemCount`, `SetMaxThreads` |
| `Timer` | `Timer(callback, state, dueTime, period)`, `Change`, `Dispose`; `CancellationTokenSource.CancelAfter` |
| `Task`, `Task<T>`, `TaskCompletionSource<T>`, `TaskStatus` (namespace `Threading.Tasks`) | `Run`, `Start`, `Wait`, `Result`, `Status`, `Exception`, `ContinueWith`, `WhenAll`, `WhenAny`, `WaitAll`, `WaitAny`, `Delay`, `FromResult`, `CompletedTask` |
| `Parallel` | `For`, `ForEach`, `Invoke` |
| `ConcurrentQueue<T>`, `ConcurrentStack<T>`, `ConcurrentBag<T>`, `ConcurrentDictionary<K,V>`, `BlockingCollection<T>` (namespace `Collections.Concurrent`) | `TryDequeue`, `TryPop`, `TryTake`, `TryAdd`, `GetOrAdd`, `AddOrUpdate`, `Take`, `CompleteAdding`, `GetConsumingEnumerable`... |
| `AggregateException` (`System`) | `InnerExceptions`, `Flatten` |
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

## The pool, the timers and the tasks

* **The pool** (`ThreadPool.cs`): a queue and background threads. A thread is made when there is more queued work than idle
  threads (the work can wait for other work, so the pool grows, up to 64 threads, instead of blocking), and ends after 3 seconds
  without work. An exception that nothing catches in a work item ends the program.
* **The timers** (`Timer.cs`): one background thread waits for the earliest due time (`Monitor.Wait` with the time); the call is
  queued to the pool, so a slow callback does not delay the others. A period is counted from the moment of the call.
* **The tasks** (`Threading/Tasks`): a task runs once in the pool. It ends ran to completion, faulted (an `AggregateException`)
  or canceled (it threw `OperationCanceledException`, or its token was canceled before it began). `Wait`, `Result` and
  `WaitAll` throw the `AggregateException`; what `ContinueWith` takes runs in the pool when the task has ended, in any state.
  `WhenAll`, `WhenAny` and `Delay` make tasks that nothing runs, that are ended by the other tasks and by a timer.
* **`Parallel`**: the thread that calls works with at most `MaxDegreeOfParallelism - 1` tasks of the pool (`ParallelOptions`; -1 is
  `Environment.ProcessorCount`); the iterations are taken one at a time; the first exception stops the loop. A body can have a
  `ParallelLoopState` (`Break()`: the iterations after this one do not begin and the ones before it run; `Stop()`: no iteration begins;
  `ShouldExitCurrentIteration`, `IsStopped`, `LowestBreakIteration`). `For`, `ForEach` and `Invoke` return a `ParallelLoopResult`
  (`IsCompleted`, `LowestBreakIteration`, -1 when no iteration broke); a canceled `ParallelOptions.CancellationToken` throws
  `OperationCanceledException`. (No `Int64` loops, no `ParallelLoopState` for `Invoke`, no `ForEach` of a partitioner.)
* **`ContinueWith`** has the overloads with `TaskContinuationOptions` (`OnlyOnFaulted`, `NotOnRanToCompletion`, `OnlyOnCanceled`...,
  `ExecuteSynchronously`: in the thread that ended the antecedent) and with a `CancellationToken`; a continuation that the options do
  not allow, or whose token is canceled, does not run and its task is canceled.
* **`ExecutionContext` and `AsyncLocal<T>`**: the value of an `AsyncLocal<T>` goes with the flow of the work. A thread has the values
  of the code that started it (`Thread.Start`), a work item of the pool the ones of the code that queued it, a timer the ones of the
  code that made it, a continuation of `ContinueWith` the ones of the code that asked for it, and the code after an `await` the ones
  it had before it (`TaskAwaiter.OnCompleted`). A change is seen only by what is started after it (the context is not changed, a
  set makes a new one): a change inside a task is not seen by the code that started it. `ExecutionContext.Capture()` and
  `Run(context, method)` are public; there is no `SuppressFlow`, and no notification when a value changes.
* **The concurrent collections** hold the lock of the object for each call; the enumerators go through a copy. The factories
  of `GetOrAdd` and `AddOrUpdate` run out of the lock.

`async` / `await`: see `docs/async.md`.

## Parallel threads (phase 4): the native programs and the JIT

The interpreter keeps the global lock for all the time (one managed thread runs at a time). A native program (compiled or JIT) that
starts a thread runs the code of the threads **in parallel**: `Thread.IsParallel` is true. The lock is not given up: it is held only
while a thread is in the runtime.

* **The runtime and the program.** A call of the runtime (`april_native_new_object`, `new_array`, `invoke` of the functions of the
  library, the threads and the locks, the reflection, `thread_static`, the enums) is made between `runtime_enter` and `runtime_leave`
  (`runtime_guard_t`); they nest. The first takes the global lock, the last gives it. Method.Invoke gives it for the time of the
  method (`runtime_suspend` / `runtime_resume`). So the heap, the tables of the collector, the monitors and the registry of the threads
  are used by one thread at a time, and the code of the program (the loops, the fields, the calls of the program) is not locked.
  A program that has not started a thread does not use the lock (`thread_parallel` is false: the guards do nothing).
* **The collection stops the world.** The thread that collects (it is in the runtime) sets the flag of the safepoints and waits until
  no thread runs the code of the program (`stop_world`): a thread that is in the runtime or waits for the lock is stopped (its stack and
  registers were kept when it entered, `thread_stopping`), a thread that runs reads the flag at a safepoint and stops there. The
  safepoints are the beginning of every function and the jumps that go back.
* **The pending exception** is a variable of each thread (`april_native_exception_slot`); the function `april.exc_slot` of the program
  gives it (for a program that starts no thread it is the constant that is inlined). The collector reads it as a root.
* **Interlocked** on numbers is made of atomic instructions (`atomicrmw`, `cmpxchg`); the interpreter calls the same functions of the
  library (`__atomic_*`). `SpinLock` uses `CompareExchange`.
* What the code of the program does with the fields of the objects is not locked: a program that shares data between threads needs
  `lock`, `Interlocked` or `Volatile` (the loads and the stores are not atomic instructions).

## Not done yet

`TaskScheduler`, `TaskFactory`, `SynchronizationContext`, `PeriodicTimer`, `Thread.Interrupt`.
`Thread.Abort`, priorities and the affinity are not planned. The interpreter runs one thread at a time (the runtime is not made to be used in parallel: the commands, the lazy tables, the string
pool). A condition expression `c? a : b` whose value is a struct of a generic param and that has a call as a branch
is not supported.
