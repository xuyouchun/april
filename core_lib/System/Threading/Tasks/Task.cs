/*
 *  System.dll
 *
 *  Task, Task<T>, TaskCompletionSource<T>: the work that runs in the thread pool, and what waits for it.
 */

using Collections;

namespace Threading.Tasks;

////////// ////////// ////////// ////////// //////////

// Where a task is.
public enum TaskStatus
{
    Created = 0,            // Made, not started.
    WaitingToRun = 1,       // Started, waits for a thread of the pool.
    Running = 2,
    RanToCompletion = 3,
    Canceled = 4,
    Faulted = 5,
}

// A task was canceled: Wait and Result throw it inside an AggregateException.
public class TaskCanceledException : OperationCanceledException
{
    public TaskCanceledException() { }

    public TaskCanceledException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// A piece of work that runs in the thread pool, once. It ends ran to completion, faulted (the exception is an
// AggregateException that Wait throws) or canceled (it threw OperationCanceledException, or its token was canceled before it
// began). What was given to ContinueWith runs when it has ended, in any of the three states.
public class Task
{
    private TaskStatus __status;
    private Exception? __error;
    private Delegate<Void> __action;
    private Boolean __hasAction;
    private CancellationToken __token;
    private List<Delegate<Void>>? __completions;
    private Int32 __id;

    private static Int32 __lastId;

    // A task that runs the method when Start is called.
    public Task(Delegate<Void> action) : this(action, CancellationToken.None)
    {
    }

    public Task(Delegate<Void> action, CancellationToken cancellationToken)
    {
        __action = action;
        __hasAction = true;
        __token = cancellationToken;
        __id = Interlocked.Increment(ref __lastId);
    }

    // A task whose method is the one of a derived class (Task<T>).
    internal Task(CancellationToken cancellationToken)
    {
        __hasAction = true;
        __token = cancellationToken;
        __id = Interlocked.Increment(ref __lastId);
    }

    // A task that nothing runs: it ends when something finishes it (TaskCompletionSource, WhenAll...).
    internal Task(Boolean promise)
    {
        __status = TaskStatus.WaitingToRun;
        __id = Interlocked.Increment(ref __lastId);
    }

    public Int32 Id => __id;

    public TaskStatus Status => __status;

    public Boolean IsCompleted => __status >= TaskStatus.RanToCompletion;

    public Boolean IsCompletedSuccessfully => __status == TaskStatus.RanToCompletion;

    public Boolean IsCanceled => __status == TaskStatus.Canceled;

    public Boolean IsFaulted => __status == TaskStatus.Faulted;

    // The exception of a task that faulted, null for the others.
    public AggregateException? Exception => __error as AggregateException;

    // Queues the task to the pool.
    public Void Start()
    {
        lock (this)
        {
            if (!__hasAction || __status != TaskStatus.Created)
                throw new InvalidOperationException("the task has been started, or it has no method to run");

            __status = TaskStatus.WaitingToRun;
        }

        Delegate<Void> execute = __Execute;
        ThreadPool.QueueUserWorkItem(execute);
    }

    // Waits for the end of the task: throws the AggregateException of a task that faulted or was canceled.
    public Void Wait()
    {
        Wait(-1);
    }

    // Waits for some milliseconds (-1 is for ever): returns whether the task ended.
    public Boolean Wait(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        lock (this)
        {
            if (__status == TaskStatus.Created)
                throw new InvalidOperationException("the task has not been started");

            Int64 end = Waiting.End(millisecondsTimeout);

            while (!IsCompleted)
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                Monitor.Wait(this, left);
            }
        }

        __ThrowIfNotSuccessful();
        return true;
    }

    public Boolean Wait(TimeSpan timeout)
    {
        return Wait(Waiting.ToMilliseconds(timeout));
    }

    // Waits for the end of all the tasks.
    public static Void WaitAll(Array<Task> tasks)
    {
        List<Exception> errors = new List<Exception>();

        for (Int32 k = 0; k < tasks.Length; k++)
        {
            try
            {
                tasks[k].Wait();
            }
            catch (AggregateException e)
            {
                for (Int32 j = 0; j < e.InnerExceptions.Length; j++)
                    errors.Add(e.InnerExceptions[j]);
            }
        }

        if (errors.Count > 0)
            throw new AggregateException(errors.ToArray());
    }

    // Waits for the end of a task, returns its index.
    public static Int32 WaitAny(Array<Task> tasks)
    {
        Task any = WhenAny(tasks).Result;

        for (Int32 k = 0; k < tasks.Length; k++)
        {
            if (Object.ReferenceEquals(tasks[k], any))
                return k;
        }

        return -1;
    }

    // Runs the method in the pool.
    public static Task Run(Delegate<Void> action)
    {
        return Run(action, CancellationToken.None);
    }

    public static Task Run(Delegate<Void> action, CancellationToken cancellationToken)
    {
        Task task = new Task(action, cancellationToken);
        task.Start();

        return task;
    }

    // Runs the method in the pool, the result of the task is what it returns.
    public static Task<T> Run<T>(Delegate<T> function)
    {
        return Run<T>(function, CancellationToken.None);
    }

    public static Task<T> Run<T>(Delegate<T> function, CancellationToken cancellationToken)
    {
        Task<T> task = new Task<T>(function, cancellationToken);
        task.Start();

        return task;
    }

    // A task that has ended with the result.
    public static Task<T> FromResult<T>(T result)
    {
        Task<T> task = new Task<T>(true);
        task.__TrySetResult(result);

        return task;
    }

    // A task that has ended.
    public static Task CompletedTask
    {
        get
        {
            Task task = new Task(true);
            task.__Finish(TaskStatus.RanToCompletion, null);

            return task;
        }
    }

    // A task that ends after the time.
    public static Task Delay(Int32 millisecondsDelay)
    {
        return Delay(millisecondsDelay, CancellationToken.None);
    }

    public static Task Delay(TimeSpan delay)
    {
        return Delay(Waiting.ToMilliseconds(delay), CancellationToken.None);
    }

    // A task that ends after the time, or is canceled when the token is.
    public static Task Delay(Int32 millisecondsDelay, CancellationToken cancellationToken)
    {
        Waiting.Check(millisecondsDelay);

        Task task = new Task(true);
        if (cancellationToken.IsCancellationRequested)
        {
            task.__Finish(TaskStatus.Canceled, null);
            return task;
        }

        if (millisecondsDelay == 0)
        {
            task.__Finish(TaskStatus.RanToCompletion, null);
            return task;
        }

        DelayState state = new DelayState();
        state.Task = task;

        Delegate<Void, Object> fire = state.Fire;
        Timer timer = new Timer(fire, null, millisecondsDelay, -1);
        state.Timer = timer;

        if (cancellationToken.CanBeCanceled)
        {
            Delegate<Void> cancel = state.Cancel;
            state.Registration = cancellationToken.Register(cancel);
        }

        return task;
    }

    // A task that ends when all the tasks have ended: ran to completion if they all did, faulted (with the exceptions of all) if
    // one did, canceled if one was.
    public static Task WhenAll(Array<Task> tasks)
    {
        Task result = new Task(true);
        if (tasks.Length == 0)
        {
            result.__Finish(TaskStatus.RanToCompletion, null);
            return result;
        }

        WhenAllState state = new WhenAllState();
        state.Tasks = tasks;
        state.Remaining = tasks.Length;
        state.Result = result;

        Delegate<Void> done = state.Done;
        for (Int32 k = 0; k < tasks.Length; k++)
            tasks[k].__OnCompleted(done);

        return result;
    }

    // The same for tasks that have a result: the result is the array of their results, in their order.
    public static Task<Array<T>> WhenAll<T>(Array<Task<T>> tasks)
    {
        Task<Array<T>> result = new Task<Array<T>>(true);
        if (tasks.Length == 0)
        {
            result.__TrySetResult(new T[0]);
            return result;
        }

        WhenAllState<T> state = new WhenAllState<T>();
        state.Tasks = tasks;
        state.Remaining = tasks.Length;
        state.Result = result;

        Delegate<Void> done = state.Done;
        for (Int32 k = 0; k < tasks.Length; k++)
            tasks[k].__OnCompleted(done);

        return result;
    }

    // A task that ends when one of the tasks has ended, its result is that task.
    public static Task<Task> WhenAny(Array<Task> tasks)
    {
        Task<Task> result = new Task<Task>(true);

        WhenAnyState state = new WhenAnyState();
        state.Tasks = tasks;
        state.Result = result;

        Delegate<Void> done = state.Done;
        for (Int32 k = 0; k < tasks.Length; k++)
            tasks[k].__OnCompleted(done);

        return result;
    }

    // Runs the method when the task has ended, in the pool; the new task is what the method does.
    public Task ContinueWith(Delegate<Void, Task> continuation)
    {
        Task next = new Task(true);

        ContinueVoid<Task> state = new ContinueVoid<Task>();
        state.Antecedent = this;
        state.Method = continuation;
        state.Next = next;

        Delegate<Void> queue = state.Queue;
        __OnCompleted(queue);

        return next;
    }

    public Task<TResult> ContinueWith<TResult>(Delegate<TResult, Task> continuation)
    {
        Task<TResult> next = new Task<TResult>(true);

        ContinueResult<Task, TResult> state = new ContinueResult<Task, TResult>();
        state.Antecedent = this;
        state.Method = continuation;
        state.Next = next;

        Delegate<Void> queue = state.Queue;
        __OnCompleted(queue);

        return next;
    }

    // The method of the task: the one of the task of a result sets the result.
    internal virtual Void __Invoke()
    {
        __action();
    }

    // Runs the callback when the task has ended (at once if it did), in the thread that ends it.
    internal Void __OnCompleted(Delegate<Void> callback)
    {
        lock (this)
        {
            if (!IsCompleted)
            {
                if (__completions == null)
                    __completions = new List<Delegate<Void>>();

                __completions.Add(callback);
                return;
            }
        }

        callback();
    }

    // Ends the task: returns false when it had ended.
    internal Boolean __Finish(TaskStatus status, Exception? error)
    {
        List<Delegate<Void>>? callbacks;

        lock (this)
        {
            if (!__BeginFinish(status, error, out callbacks))
                return false;
        }

        __RunCompletions(callbacks);
        return true;
    }

    // Ends the task; the lock of the task is held. The callbacks to run (after the lock is given back) are returned.
    internal Boolean __BeginFinish(TaskStatus status, Exception? error, out List<Delegate<Void>>? callbacks)
    {
        callbacks = null;

        if (IsCompleted)
            return false;

        __status = status;
        __error = error;

        callbacks = __completions;
        __completions = null;

        Monitor.PulseAll(this);
        return true;
    }

    internal static Void __RunCompletions(List<Delegate<Void>>? callbacks)
    {
        if (callbacks == null)
            return;

        for (Int32 k = 0; k < callbacks.Count; k++)
        {
            Delegate<Void> callback = callbacks[k];
            callback();
        }
    }

    // Throws the AggregateException of a task that has not ran to completion.
    internal Void __ThrowIfNotSuccessful()
    {
        if (__status == TaskStatus.Faulted)
            throw __error;

        if (__status == TaskStatus.Canceled)
            throw new AggregateException(new TaskCanceledException("the task was canceled"));
    }

    // The thread of the pool runs the task.
    private Void __Execute()
    {
        if (__token.IsCancellationRequested)
        {
            __Finish(TaskStatus.Canceled, null);
            return;
        }

        lock (this)
            __status = TaskStatus.Running;

        try
        {
            __Invoke();
        }
        catch (OperationCanceledException e)
        {
            __Finish(TaskStatus.Canceled, null);
            return;
        }
        catch (Exception e)
        {
            __Finish(TaskStatus.Faulted, new AggregateException(e));
            return;
        }

        __Finish(TaskStatus.RanToCompletion, null);
    }
}

////////// ////////// ////////// ////////// //////////

// A task that has a result.
public class Task<T> : Task
{
    private T __result;
    private Delegate<T> __function;

    public Task(Delegate<T> function) : base(CancellationToken.None)
    {
        __function = function;
    }

    public Task(Delegate<T> function, CancellationToken cancellationToken) : base(cancellationToken)
    {
        __function = function;
    }

    internal Task(Boolean promise) : base(promise)
    {
    }

    // The result: waits for the end of the task, throws the AggregateException of a task that did not ran to completion.
    public T Result
    {
        get
        {
            Wait();
            return __result;
        }
    }

    // Sets the result and ends the task, returns false when it had ended.
    internal Boolean __TrySetResult(T value)
    {
        List<Delegate<Void>>? callbacks;

        lock (this)
        {
            if (IsCompleted)
                return false;

            __result = value;
            __BeginFinish(TaskStatus.RanToCompletion, null, out callbacks);
        }

        __RunCompletions(callbacks);
        return true;
    }

    internal override Void __Invoke()
    {
        __result = __function();
    }

    // Runs the method when the task has ended, in the pool.
    public Task ContinueWith(Delegate<Void, Task<T>> continuation)
    {
        Task next = new Task(true);

        ContinueVoid<Task<T>> state = new ContinueVoid<Task<T>>();
        state.Antecedent = this;
        state.Method = continuation;
        state.Next = next;

        Delegate<Void> queue = state.Queue;
        __OnCompleted(queue);

        return next;
    }

    public Task<TResult> ContinueWith<TResult>(Delegate<TResult, Task<T>> continuation)
    {
        Task<TResult> next = new Task<TResult>(true);

        ContinueResult<Task<T>, TResult> state = new ContinueResult<Task<T>, TResult>();
        state.Antecedent = this;
        state.Method = continuation;
        state.Next = next;

        Delegate<Void> queue = state.Queue;
        __OnCompleted(queue);

        return next;
    }
}
