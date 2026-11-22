/*
 *  System.dll
 *
 *  What the tasks use: the states of Delay, WhenAll, WhenAny, ContinueWith, and TaskCompletionSource<T>.
 */

using Collections;

namespace Threading.Tasks;

////////// ////////// ////////// ////////// //////////

// The state of Task.Delay: the timer ends the task, the token cancels it.
internal sealed class DelayState
{
    internal Task Task;
    internal Timer Timer;
    internal CancellationTokenRegistration Registration;

    internal Void Fire(Object state)
    {
        Timer.Dispose();
        Registration.Dispose();
        Task.__Finish(TaskStatus.RanToCompletion, null);
    }

    internal Void Cancel()
    {
        Timer.Dispose();
        Task.__Finish(TaskStatus.Canceled, null);
    }
}

////////// ////////// ////////// ////////// //////////

// The state of Task.WhenAll.
internal sealed class WhenAllState
{
    internal Array<Task> Tasks;
    internal Int32 Remaining;
    internal Task Result;

    internal Void Done()
    {
        lock (this)
        {
            Remaining--;
            if (Remaining > 0)
                return;
        }

        List<Exception> errors = new List<Exception>();
        Boolean canceled = false;

        for (Int32 k = 0; k < Tasks.Length; k++)
        {
            if (Tasks[k].IsFaulted)
            {
                AggregateException? aggregate = Tasks[k].Exception;
                for (Int32 j = 0; j < aggregate.InnerExceptions.Length; j++)
                    errors.Add(aggregate.InnerExceptions[j]);
            }
            else if (Tasks[k].IsCanceled)
            {
                canceled = true;
            }
        }

        if (errors.Count > 0)
            Result.__Finish(TaskStatus.Faulted, new AggregateException(errors.ToArray()));
        else if (canceled)
            Result.__Finish(TaskStatus.Canceled, null);
        else
            Result.__Finish(TaskStatus.RanToCompletion, null);
    }
}

// The state of Task.WhenAll<T>: the results of the tasks, in their order.
internal sealed class WhenAllState<T>
{
    internal Array<Task<T>> Tasks;
    internal Int32 Remaining;
    internal Task<Array<T>> Result;

    internal Void Done()
    {
        lock (this)
        {
            Remaining--;
            if (Remaining > 0)
                return;
        }

        List<Exception> errors = new List<Exception>();
        Boolean canceled = false;
        T[] values = new T[Tasks.Length];

        for (Int32 k = 0; k < Tasks.Length; k++)
        {
            if (Tasks[k].IsFaulted)
            {
                AggregateException? aggregate = Tasks[k].Exception;
                for (Int32 j = 0; j < aggregate.InnerExceptions.Length; j++)
                    errors.Add(aggregate.InnerExceptions[j]);
            }
            else if (Tasks[k].IsCanceled)
            {
                canceled = true;
            }
            else
            {
                values[k] = Tasks[k].Result;
            }
        }

        if (errors.Count > 0)
            Result.__Finish(TaskStatus.Faulted, new AggregateException(errors.ToArray()));
        else if (canceled)
            Result.__Finish(TaskStatus.Canceled, null);
        else
            Result.__TrySetResult(values);
    }
}

// The state of Task.WhenAny.
internal sealed class WhenAnyState
{
    internal Array<Task> Tasks;
    internal Task<Task> Result;

    internal Void Done()
    {
        for (Int32 k = 0; k < Tasks.Length; k++)
        {
            if (Tasks[k].IsCompleted)
            {
                Result.__TrySetResult(Tasks[k]);
                return;
            }
        }
    }
}

////////// ////////// ////////// ////////// //////////

// The conditions of a continuation (TaskContinuationOptions): whether the antecedent allows it to run. The continuation does not run
// (its task is canceled) when the antecedent ended as an option says (NotOn...), or the token was canceled.
internal static class ContinueRules
{
    internal static Boolean Allows(Task antecedent, Int32 options, CancellationToken token)
    {
        if (token.IsCancellationRequested)
            return false;

        if ((options & 0x10000) != 0 && antecedent.IsCompletedSuccessfully)
            return false;

        if ((options & 0x20000) != 0 && antecedent.IsFaulted)
            return false;

        if ((options & 0x40000) != 0 && antecedent.IsCanceled)
            return false;

        return true;
    }
}

// ContinueWith of a method that has no result: it is queued to the pool when the task has ended.
internal sealed class ContinueVoid<TTask>
{
    internal ExecutionContext? Context = ExecutionContext.Capture();
    internal TTask Antecedent;
    internal Task Base;
    internal Int32 Options;
    internal CancellationToken Token;
    internal Delegate<Void, TTask> Method;
    internal Task Next;

    internal Void Queue()
    {
        if (!ContinueRules.Allows(Base, Options, Token))
        {
            Next.__Finish(TaskStatus.Canceled, null);
            return;
        }

        Delegate<Void> run = Run;

        // The continuation has the context of the code that asked for it (ContinueWith), not the one of the thread that ended the task.
        ExecutionContext? saved = ExecutionContext.__Swap(Context);

        try
        {
            // ExecuteSynchronously: in the thread that ended the antecedent.
            if ((Options & 0x80000) != 0)
                run();
            else
                ThreadPool.QueueUserWorkItem(run);
        }
        finally
        {
            ExecutionContext.__Swap(saved);
        }
    }

    private Void Run()
    {
        try
        {
            Method(Antecedent);
        }
        catch (OperationCanceledException e)
        {
            Next.__Finish(TaskStatus.Canceled, null);
            return;
        }
        catch (Exception e)
        {
            Next.__Finish(TaskStatus.Faulted, new AggregateException(e));
            return;
        }

        Next.__Finish(TaskStatus.RanToCompletion, null);
    }
}

// ContinueWith of a method that has a result.
internal sealed class ContinueResult<TTask, TResult>
{
    internal ExecutionContext? Context = ExecutionContext.Capture();
    internal TTask Antecedent;
    internal Task Base;
    internal Int32 Options;
    internal CancellationToken Token;
    internal Delegate<TResult, TTask> Method;
    internal Task<TResult> Next;

    internal Void Queue()
    {
        if (!ContinueRules.Allows(Base, Options, Token))
        {
            Next.__Finish(TaskStatus.Canceled, null);
            return;
        }

        Delegate<Void> run = Run;

        // The continuation has the context of the code that asked for it (ContinueWith), not the one of the thread that ended the task.
        ExecutionContext? saved = ExecutionContext.__Swap(Context);

        try
        {
            // ExecuteSynchronously: in the thread that ended the antecedent.
            if ((Options & 0x80000) != 0)
                run();
            else
                ThreadPool.QueueUserWorkItem(run);
        }
        finally
        {
            ExecutionContext.__Swap(saved);
        }
    }

    private Void Run()
    {
        TResult value;

        try
        {
            value = Method(Antecedent);
        }
        catch (OperationCanceledException e)
        {
            Next.__Finish(TaskStatus.Canceled, null);
            return;
        }
        catch (Exception e)
        {
            Next.__Finish(TaskStatus.Faulted, new AggregateException(e));
            return;
        }

        Next.__TrySetResult(value);
    }
}

////////// ////////// ////////// ////////// //////////

// Makes a task that something finishes: the result, an exception, or the cancellation.
public class TaskCompletionSource<T>
{
    private Task<T> __task = new Task<T>(true);

    public TaskCompletionSource()
    {
    }

    // The task.
    public Task<T> Task => __task;

    public Void SetResult(T result)
    {
        if (!__task.__TrySetResult(result))
            throw new InvalidOperationException("the task has ended");
    }

    public Boolean TrySetResult(T result)
    {
        return __task.__TrySetResult(result);
    }

    public Void SetException(Exception exception)
    {
        if (!TrySetException(exception))
            throw new InvalidOperationException("the task has ended");
    }

    public Boolean TrySetException(Exception exception)
    {
        return __task.__Finish(TaskStatus.Faulted, new AggregateException(exception));
    }

    public Void SetCanceled()
    {
        if (!TrySetCanceled())
            throw new InvalidOperationException("the task has ended");
    }

    public Boolean TrySetCanceled()
    {
        return __task.__Finish(TaskStatus.Canceled, null);
    }
}
