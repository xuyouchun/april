/*
 *  System.dll
 *
 *  CancellationTokenSource, CancellationToken: the way to tell an operation that it must stop.
 */

using Collections;

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A method that runs when the source is canceled.
internal sealed class CancellationCallback
{
    internal Delegate<Void> Action;
    internal Boolean Active = true;

    internal CancellationCallback(Delegate<Void> action)
    {
        Action = action;
    }
}

// What cancels the tokens that it gives: Cancel() once, the tokens tell it and run what was registered.
public class CancellationTokenSource : IDisposable
{
    private Boolean __canceled;
    private Boolean __disposed;
    private List<CancellationCallback> __callbacks = new List<CancellationCallback>();

    public CancellationTokenSource()
    {
    }

    public Boolean IsCancellationRequested => __canceled;

    // The token that tells it.
    public CancellationToken Token
    {
        get
        {
            if (__disposed)
                throw new ObjectDisposedException("the source is disposed");

            return new CancellationToken(this);
        }
    }

    // Cancels: the methods that were registered run (in the order of the registrations), in this thread. An exception of one
    // does not stop the others: the first is thrown at the end.
    public Void Cancel()
    {
        CancellationCallback[] callbacks;

        lock (this)
        {
            if (__canceled)
                return;

            __canceled = true;
            callbacks = __callbacks.ToArray();
            __callbacks = new List<CancellationCallback>();
        }

        Exception? first = null;

        for (Int32 k = 0; k < callbacks.Length; k++)
        {
            CancellationCallback callback = callbacks[k];
            if (!callback.Active)
                continue;

            try
            {
                Delegate<Void> action = callback.Action;
                action();
            }
            catch (Exception e)
            {
                if (first == null)
                    first = e;
            }
        }

        if (first != null)
            throw first;
    }

    // A source that is canceled when one of the tokens is.
    public static CancellationTokenSource CreateLinkedTokenSource(CancellationToken token1, CancellationToken token2)
    {
        CancellationTokenSource linked = new CancellationTokenSource();

        Delegate<Void> cancel = linked.__CancelLinked;
        token1.Register(cancel);
        token2.Register(cancel);

        return linked;
    }

    private Void __CancelLinked()
    {
        Cancel();
    }

    // Registers a method: it runs at once when the source is canceled already.
    internal CancellationTokenRegistration __Register(Delegate<Void> callback)
    {
        CancellationCallback item = new CancellationCallback(callback);

        lock (this)
        {
            if (!__canceled)
            {
                __callbacks.Add(item);
                return new CancellationTokenRegistration(item);
            }
        }

        callback();
        return new CancellationTokenRegistration(null);
    }

    public Void Dispose()
    {
        __disposed = true;
    }
}

////////// ////////// ////////// ////////// //////////

// The registration of a method: Dispose takes it back.
public struct CancellationTokenRegistration : IDisposable
{
    private CancellationCallback? __callback;

    internal CancellationTokenRegistration(CancellationCallback? callback)
    {
        __callback = callback;
    }

    // The method does not run any more (if it did not yet).
    public Void Dispose()
    {
        if (__callback != null)
            __callback.Active = false;
    }
}

////////// ////////// ////////// ////////// //////////

// Tells an operation that it is canceled.
public struct CancellationToken
{
    private CancellationTokenSource? __source;

    internal CancellationToken(CancellationTokenSource source)
    {
        __source = source;
    }

    // A token that is never canceled.
    public static CancellationToken None => new CancellationToken();

    public Boolean CanBeCanceled => __source != null;

    public Boolean IsCancellationRequested => __source != null && __source.IsCancellationRequested;

    // Throws OperationCanceledException when the source is canceled.
    public Void ThrowIfCancellationRequested()
    {
        if (IsCancellationRequested)
            throw new OperationCanceledException("the operation was canceled");
    }

    // Registers a method that runs when the source is canceled (at once if it was).
    public CancellationTokenRegistration Register(Delegate<Void> callback)
    {
        if (__source == null)
            return new CancellationTokenRegistration(null);

        return __source.__Register(callback);
    }
}
