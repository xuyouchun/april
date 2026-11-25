# async / await

There is no `async` keyword. `await` is a reserved word, and the **return type of the method** says what it means.

| The method returns | and its body has `await` | what `await e` is |
|---|---|---|
| `Task`, `Task<T>` | yes | the method is a state machine (below) |
| `Task`, `Task<T>` | no | nothing changes: an ordinary method that returns a task |
| anything else | yes | `e.GetAwaiter().GetResult()`: it waits in the thread |
| `IAsyncEnumerable<T>` / `IAsyncEnumerator<T>` with `yield` | yes or no | an async iterator (below) |

Only the body of the method counts: the `await` of a lambda belongs to the lambda (whose return type decides, the same way).

## A method that returns a task

```
static Task<Int32> Sum()
{
    Int32 a = await Compute(1);
    Int32 b = await Compute(2);
    return a + b;               // T, not Task<T>
}
```

* `return x` gives the value to the task (`return;` in a method that returns `Task`). A method that has `await` and returns a
  value that is a task must write the value: `return Task.FromResult(1)` is an error there.
* An exception that the method throws (all of them, also the ones of the checks of the params) is the one of the task: the call
  does not throw. `await` throws the exception **as it is**, not inside an `AggregateException` (`Wait` and `Result` still do).
  An `OperationCanceledException` cancels the task. A method that has no `await` throws at the call, as any method.
* The method runs in the thread of the caller until the first `await` of a task that has not ended. Then it keeps its state,
  asks the awaiter to call it again (`OnCompleted`), and returns the task. The continuation is **queued to the thread pool**.
  `await` of a task that has ended goes on at once, without a thread switch. There is no synchronization context.
* `Task Main()` and `Task<Int32> Main()` are entry points: the program waits for the task.

### How it is made

The compiler makes it like the iterators (`docs/compiler-generated.md`): a state class that has the params, the locals (as fields),
the builder of the task (`System.Threading.Tasks.AsyncTaskBuilder`) and the awaiters; the body goes to a method that does the work,
called by a method that gives the exception to the task. The same code is made for the interpreter, the JIT and the native programs.

At the beginning of each statement that has awaits (where nothing is on the stack of the evaluation) the compiler puts, for each
await (the inner ones first): `awaiter = task.GetAwaiter(); if (!awaiter.IsCompleted) { state = n; awaiter.OnCompleted(callback); return; }`
and the code goes on at the label `n` of the dispatch at the beginning of the method. The value of the await is `awaiter.GetResult()`.
The `return` of the suspension does not run the `finally` blocks it is in (it is not a `leave`); the dispatch jumps into the
`try` blocks.

An awaitable is any type that has `GetAwaiter()`, whose result has `IsCompleted`, `OnCompleted(Delegate<Void>)` and `GetResult()`
(`Task`, `Task<T>`: `TaskAwaiter`, `TaskAwaiter<T>`).

### Limits (errors AP5208)

* `await` is not allowed in a `catch` or `finally` block, nor in a `lock` (the lock belongs to a thread, the method goes on in
  another): use `using (await semaphore.LockAsync()) { ... }` (`SemaphoreSlim`).
* `await` is not allowed in the part of an expression that does not always run: the right side of `&&`, `||`, `??`, and the branches of `?:`
  (the waits are made at the beginning of the statement, in order; the other operands are evaluated after them).
* Generic methods, `ref` and `out` params are not supported.
* A lambda cannot be a state machine yet: its `await` waits in the thread.
* `async void` does not exist: return `Task`.
* The wait of a method that is not a state machine (`await` in `Main`, in a constructor, in a lambda) holds the thread: the pool
  grows when work is queued and no thread is idle (up to its maximum), but waiting for a task that needs a thread when the pool is
  full never ends.

## Sequences: `IAsyncEnumerable<T>`

```
static IAsyncEnumerable<Int32> Numbers(Int32 count)
{
    for (Int32 i = 1; i <= count; i++)
        yield return await Next(i);        // yield and await: an async iterator
}

await foreach (Int32 n in Numbers(3)) { ... }
```

* An async iterator is a state machine too: `MoveNextAsync()` makes the task of the next item and runs the method until the next
  `yield return` (the task ends with `true`) or its end (`false`). `return` is not allowed in it (use `yield break`).
* `await foreach` is `GetAsyncEnumerator()`, `await MoveNextAsync()`, `Current`; `DisposeAsync()` is waited for **in the thread**
  when the loop ends (a `finally` block cannot wait). Like `await`, it waits in the thread in a method that is not a state machine.
* The interfaces are `Collections.IAsyncEnumerable<T>` and `IAsyncEnumerator<T>`.

## The library

The files and the streams have `Async` methods (`docs/io.md`), the sockets too (`docs/net.md`). `Task.GetAwaiter()`, `Task.Yield()`, `SemaphoreSlim.WaitAsync()` / `LockAsync()` (the counts go to the waiting tasks in the order),
`AsyncTaskBuilder`, `AsyncTaskBuilder<T>` (the compiler uses them), `TaskAwaiter`, `TaskAwaiter<T>`.

## Async lambdas

`async () => body`, `async x => body`, `async (a, b) => { ... }` is a lambda that makes a task: the compiler writes it as
`(a, b) => AsyncLambda.Run(() => body)`. The body runs in a task of the thread pool, and its `await`s wait in that thread
(like `await` in a method that does not return a task). The result type of the task is the type of the body.

## Not done yet

* `ValueTask` (the awaitable protocol is duck typed, so it can be added without changing the compiler).
* The files are made by the thread pool (`docs/io.md`), the sockets by a reactor that waits for them without a thread (`docs/net.md`).
* Structured concurrency (a scope that waits for its tasks and cancels them), `Channel<T>`.
* Generic async methods, `await` in `catch` / `finally`.
