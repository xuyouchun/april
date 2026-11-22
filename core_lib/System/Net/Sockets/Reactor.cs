/*
 *  System.dll
 *
 *  SocketException, SocketReactor: the errors of the sockets, and what waits for the sockets without holding a thread each.
 */

using Collections;
using IO;
using Threading;
using Threading.Tasks;

namespace Net.Sockets;

////////// ////////// ////////// ////////// //////////

// An error of a socket: ErrorCode is the code of the runtime: -2 refused, -3 reset or closed, -4 other, -5 the address is in use,
// -6 timed out.
public class SocketException : IOException
{
    private Int32 __code;

    public SocketException(Int32 code, String? message) : base(message)
    {
        __code = code;
    }

    public Int32 ErrorCode => __code;
}

internal static class SocketError
{
    // Throws the exception of a negative code (the code of "would block" is not an error: the callers handle it).
    internal static Void Check(Int64 code, String what)
    {
        if (code >= 0)
            return;

        switch ((Int32)code)
        {
            case -2:
                throw new SocketException(-2, what + ": the connection was refused");

            case -3:
                throw new SocketException(-3, what + ": the connection was reset or closed");

            case -5:
                throw new SocketException(-5, what + ": the address is in use");

            case -6:
                throw new SocketException(-6, what + ": timed out");

            default:
                throw new SocketException(-4, what + ": error of the system");
        }
    }
}

////////// ////////// ////////// ////////// //////////

// A task that waits for a socket.
internal class SocketWaiter
{
    internal Int64 Handle;
    internal Int32 Events;
    internal TaskCompletionSource<Boolean> Source = new TaskCompletionSource<Boolean>();
}

// The thread that waits for many sockets at once (a socket that waits does not take a thread of the pool). Wait gives a task that
// ends when the socket can be read (events 1) or written (2), or has an error; the operation is made again by the caller. The
// thread begins at the first wait, it is a background thread. A byte sent to a socket of the loopback wakes it when a wait is added.
internal static class SocketReactor
{
    private static Object __gate = new Object();
    private static List<SocketWaiter> __waiters = new List<SocketWaiter>();
    private static Boolean __started;
    private static Int64 __wake;

    // The task that ends when the socket is ready (the token cancels it).
    internal static Task<Boolean> Wait(Int64 handle, Int32 events, CancellationToken token)
    {
        if (token.IsCancellationRequested)
            throw new OperationCanceledException();

        SocketWaiter waiter = new SocketWaiter();
        waiter.Handle = handle;
        waiter.Events = events;

        lock (__gate)
        {
            __Start();
            __waiters.Add(waiter);
        }

        Internal.Socket_Wake();

        if (token.CanBeCanceled)
        {
            WaiterCancel cancel = new WaiterCancel(waiter);
            Delegate<Void> run = cancel.Run;
            token.Register(run);
        }

        return waiter.Source.Task;
    }

    // Ends the waits for a socket that is closed: the callers find the error when they try again.
    internal static Void Release(Int64 handle)
    {
        List<SocketWaiter> ended = new List<SocketWaiter>();

        lock (__gate)
        {
            for (Int32 k = __waiters.Count - 1; k >= 0; k--)
            {
                if (__waiters[k].Handle == handle)
                {
                    ended.Add(__waiters[k]);
                    __waiters.RemoveAt(k);
                }
            }
        }

        for (Int32 k = 0; k < ended.Count; k++)
            ended[k].Source.TrySetResult(true);
    }

    internal static Void Cancel(SocketWaiter waiter)
    {
        Boolean removed;

        lock (__gate)
            removed = __waiters.Remove(waiter);

        if (removed)
            waiter.Source.TrySetCanceled();
    }

    // Called with the lock.
    private static Void __Start()
    {
        if (__started)
            return;

        Int64 wake = Internal.Socket_WakeHandle();
        SocketError.Check(wake, "the reactor");

        __wake = wake;
        __started = true;

        Thread thread = new Thread(__Run);
        thread.IsBackground = true;
        thread.Start();
    }

    private static Void __Run()
    {
        UInt8[] drain = new UInt8[64];

        for (;;)
        {
            SocketWaiter[] waiters;

            lock (__gate)
                waiters = __waiters.ToArray();

            Int32 n = waiters.Length + 1;
            Int64[] handles = new Int64[n];
            Int32[] events = new Int32[n];
            Int32[] results = new Int32[n];

            handles[0] = __wake;
            events[0] = 1;

            for (Int32 k = 0; k < waiters.Length; k++)
            {
                handles[k + 1] = waiters[k].Handle;
                events[k + 1] = waiters[k].Events;
            }

            Int32 ready = Internal.Socket_Poll(Ptr.AddressOf(handles), Ptr.AddressOf(events), Ptr.AddressOf(results), n, -1);
            if (ready < 0)
            {
                Thread.Sleep(10);
                continue;
            }

            // The bytes that woke the thread are taken away.
            if (results[0] != 0)
            {
                while (Internal.Socket_Receive(__wake, Ptr.AddressOf(drain), drain.Length) > 0)
                {
                }
            }

            List<SocketWaiter> fired = new List<SocketWaiter>();

            lock (__gate)
            {
                for (Int32 k = 0; k < waiters.Length; k++)
                {
                    if (results[k + 1] != 0 && __waiters.Remove(waiters[k]))
                        fired.Add(waiters[k]);
                }
            }

            for (Int32 k = 0; k < fired.Count; k++)
                fired[k].Source.TrySetResult(true);
        }
    }
}

internal class WaiterCancel
{
    private SocketWaiter __waiter;

    internal WaiterCancel(SocketWaiter waiter)
    {
        __waiter = waiter;
    }

    internal Void Run()
    {
        SocketReactor.Cancel(__waiter);
    }
}
