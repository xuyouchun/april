/*
 *  System.dll
 *
 *  AsyncTaskBuilder, AsyncTaskBuilder<T>: the task that the compiler makes for a method that waits with `await` (the method is a
 *  state machine, the builder is what it ends: SetResult for the value that it returns, SetException for the exception that it
 *  throws).
 */

namespace Threading.Tasks;

////////// ////////// ////////// ////////// //////////

// The builder of the task of a method that returns Task.
public class AsyncTaskBuilder
{
    private Task __task = new Task(true);

    // The task that the method returns.
    public Task Task => __task;

    public Void SetResult()
    {
        __task.__Finish(TaskStatus.RanToCompletion, null);
    }

    // An OperationCanceledException cancels the task, any other exception is the one of the AggregateException of the task.
    public Void SetException(Exception exception)
    {
        if (exception is OperationCanceledException)
            __task.__Finish(TaskStatus.Canceled, null);
        else
            __task.__Finish(TaskStatus.Faulted, new AggregateException(exception));
    }

    // The method goes on in the pool (the continuation of a task that has not ended).
    public static Void Schedule(Delegate<Void> run)
    {
        ThreadPool.QueueUserWorkItem(run);
    }
}

////////// ////////// ////////// ////////// //////////

// The builder of the task of a method that returns Task<T>.
public class AsyncTaskBuilder<T>
{
    private Task<T> __task = new Task<T>(true);

    public Task<T> Task => __task;

    public Void SetResult(T result)
    {
        __task.__TrySetResult(result);
    }

    public Void SetException(Exception exception)
    {
        if (exception is OperationCanceledException)
            __task.__Finish(TaskStatus.Canceled, null);
        else
            __task.__Finish(TaskStatus.Faulted, new AggregateException(exception));
    }
}
