/*
 *  System.dll
 *
 *  TaskAwaiter, TaskAwaiter<T>: what `await task` uses.
 */

namespace Threading.Tasks;

////////// ////////// ////////// ////////// //////////

// The awaiter of a task. `await task` is task.GetAwaiter().GetResult() in a method that is not asynchronous (it waits in the
// thread), and in a method that returns a task the method is a state machine that goes on when OnCompleted calls its callback.
// The exception of a task that faulted is thrown as it is (not inside the AggregateException that Wait throws); a canceled task
// throws TaskCanceledException.
public class TaskAwaiter
{
    // The method that runs the continuation in the context of the code that waits (the continuation queues the rest of the method to the
    // pool, the pool keeps the context that is set then).
    internal static Delegate<Void> ContinuationOf(Delegate<Void> continuation)
    {
        ExecutionContext? context = ExecutionContext.Capture();
        if (context == null)
            return continuation;

        ContextCall call = new ContextCall(context, continuation);
        Delegate<Void> run = call.Run;

        return run;
    }

    private Task __task;

    internal TaskAwaiter(Task task)
    {
        __task = task;
    }

    public Boolean IsCompleted => __task.IsCompleted;

    // Runs the callback when the task has ended (at once if it did), in the thread that ends it.
    public Void OnCompleted(Delegate<Void> continuation)
    {
        __task.__OnCompleted(ContinuationOf(continuation));
    }

    // Waits for the end of the task, throws its exception.
    public Void GetResult()
    {
        __task.__AwaitResult();
    }
}

////////// ////////// ////////// ////////// //////////

public class TaskAwaiter<T>
{
    private Task<T> __task;

    internal TaskAwaiter(Task<T> task)
    {
        __task = task;
    }

    public Boolean IsCompleted => __task.IsCompleted;

    public Void OnCompleted(Delegate<Void> continuation)
    {
        __task.__OnCompleted(TaskAwaiter.ContinuationOf(continuation));
    }

    // Waits for the end of the task, returns its result or throws its exception.
    public T GetResult()
    {
        return __task.__AwaitValue();
    }
}
