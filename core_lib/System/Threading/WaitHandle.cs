/*
 *  System.dll
 *
 *  WaitHandle and the events, the semaphore and the mutex that are made of it.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A thing that a thread waits for: it is signaled or not, a thread that waits for it goes on when it is. The handles share one
// lock (the gate): what a thread can wait for is changed under it, and each change wakes up the threads that wait, so a
// thread can wait for many handles at once (WaitAny, WaitAll).
public abstract class WaitHandle
{
    // The result of WaitAny when the time was over.
    public const Int32 WaitTimeout = 258;

    // The lock of all the handles (a static field with an initial value: the static constructors of the core library run when
    // the program starts).
    private static Object __gate = new Object();

    internal static Object __Gate => __gate;

    // Whether the handle is signaled for the running thread (the gate is held).
    internal abstract Boolean __IsSignaled();

    // The thread goes on: the handle changes (an auto reset event is not signaled any more, a semaphore has one less...).
    internal abstract Void __Consume();

    // Wakes up the threads that wait (the gate is held).
    internal static Void __Changed()
    {
        Monitor.PulseAll(__Gate);
    }

    // Waits until the handle is signaled.
    public Boolean WaitOne()
    {
        return WaitOne(-1);
    }

    // Waits for the handle for some milliseconds (-1 is for ever): returns whether it was signaled.
    public Boolean WaitOne(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        Object gate = __Gate;
        lock (gate)
        {
            Int64 end = Waiting.End(millisecondsTimeout);

            while (!__IsSignaled())
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                Monitor.Wait(gate, left);
            }

            __Consume();
            return true;
        }
    }

    public Boolean WaitOne(TimeSpan timeout)
    {
        return WaitOne(Waiting.ToMilliseconds(timeout));
    }

    // Waits until all the handles are signaled (at the same time).
    public static Boolean WaitAll(Array<WaitHandle> handles)
    {
        return WaitAll(handles, -1);
    }

    public static Boolean WaitAll(Array<WaitHandle> handles, Int32 millisecondsTimeout)
    {
        __CheckHandles(handles);
        Waiting.Check(millisecondsTimeout);

        Object gate = __Gate;
        lock (gate)
        {
            Int64 end = Waiting.End(millisecondsTimeout);

            while (!__AllSignaled(handles))
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                Monitor.Wait(gate, left);
            }

            for (Int32 k = 0; k < handles.Length; k++)
                handles[k].__Consume();

            return true;
        }
    }

    // Waits until a handle is signaled, returns its index (the first one that is), or WaitTimeout.
    public static Int32 WaitAny(Array<WaitHandle> handles)
    {
        return WaitAny(handles, -1);
    }

    public static Int32 WaitAny(Array<WaitHandle> handles, Int32 millisecondsTimeout)
    {
        __CheckHandles(handles);
        Waiting.Check(millisecondsTimeout);

        Object gate = __Gate;
        lock (gate)
        {
            Int64 end = Waiting.End(millisecondsTimeout);

            for (;;)
            {
                for (Int32 k = 0; k < handles.Length; k++)
                {
                    if (handles[k].__IsSignaled())
                    {
                        handles[k].__Consume();
                        return k;
                    }
                }

                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return WaitTimeout;

                Monitor.Wait(gate, left);
            }
        }
    }

    private static Boolean __AllSignaled(Array<WaitHandle> handles)
    {
        for (Int32 k = 0; k < handles.Length; k++)
        {
            if (!handles[k].__IsSignaled())
                return false;
        }

        return true;
    }

    private static Void __CheckHandles(Array<WaitHandle> handles)
    {
        if (handles == null || handles.Length == 0)
            throw new ArgumentException("there is no wait handle");
    }
}

////////// ////////// ////////// ////////// //////////

// An event that stays signaled until Reset is called: all the threads that wait go on.
public sealed class ManualResetEvent : WaitHandle
{
    private Boolean __state;

    public ManualResetEvent(Boolean initialState)
    {
        __state = initialState;
    }

    // Signals the event: the threads that wait go on.
    public Boolean Set()
    {
        lock (WaitHandle.__Gate)
        {
            __state = true;
            WaitHandle.__Changed();
        }

        return true;
    }

    // Makes the event not signaled.
    public Boolean Reset()
    {
        lock (WaitHandle.__Gate)
            __state = false;

        return true;
    }

    internal override Boolean __IsSignaled() => __state;

    internal override Void __Consume() { }
}

////////// ////////// ////////// ////////// //////////

// An event that goes back to not signaled when a thread went on: one thread goes on for each Set.
public sealed class AutoResetEvent : WaitHandle
{
    private Boolean __state;

    public AutoResetEvent(Boolean initialState)
    {
        __state = initialState;
    }

    public Boolean Set()
    {
        lock (WaitHandle.__Gate)
        {
            __state = true;
            WaitHandle.__Changed();
        }

        return true;
    }

    public Boolean Reset()
    {
        lock (WaitHandle.__Gate)
            __state = false;

        return true;
    }

    internal override Boolean __IsSignaled() => __state;

    internal override Void __Consume()
    {
        __state = false;
    }
}

////////// ////////// ////////// ////////// //////////

// The count of a semaphore is over the maximum.
public class SemaphoreFullException : SystemException
{
    public SemaphoreFullException() { }

    public SemaphoreFullException(String? message) : base(message) { }
}

// A count of the threads that can go on: each thread that goes on takes one, Release gives them back.
public sealed class Semaphore : WaitHandle
{
    private Int32 __count;
    private Int32 __maximum;

    public Semaphore(Int32 initialCount, Int32 maximumCount)
    {
        if (maximumCount < 1 || initialCount < 0 || initialCount > maximumCount)
            throw new ArgumentException("the count of the semaphore is not valid");

        __count = initialCount;
        __maximum = maximumCount;
    }

    public Int32 Release()
    {
        return Release(1);
    }

    // Gives some counts back, returns the count before.
    public Int32 Release(Int32 releaseCount)
    {
        if (releaseCount < 1)
            throw new ArgumentException("the count is not positive");

        lock (WaitHandle.__Gate)
        {
            Int32 before = __count;
            if (__maximum - __count < releaseCount)
                throw new SemaphoreFullException("the count is over the maximum");

            __count = __count + releaseCount;
            WaitHandle.__Changed();

            return before;
        }
    }

    internal override Boolean __IsSignaled() => __count > 0;

    internal override Void __Consume()
    {
        __count = __count - 1;
    }
}

////////// ////////// ////////// ////////// //////////

// A lock that a thread takes again: WaitOne takes it (the same thread as many times as it wants), ReleaseMutex gives it back
// as many times. It is of the program, not of the machine.
public sealed class Mutex : WaitHandle
{
    private Int64 __owner;
    private Int32 __count;

    public Mutex()
    {
    }

    public Mutex(Boolean initiallyOwned)
    {
        if (initiallyOwned)
        {
            __owner = Thread.CurrentThread.ManagedThreadId;
            __count = 1;
        }
    }

    // Gives the mutex back once: throws when the thread does not own it.
    public Void ReleaseMutex()
    {
        Int64 id = Thread.CurrentThread.ManagedThreadId;

        lock (WaitHandle.__Gate)
        {
            if (__owner != id)
                throw new SynchronizationLockException("the mutex is not owned by this thread");

            __count = __count - 1;
            if (__count == 0)
            {
                __owner = 0;
                __Changed();
            }
        }
    }

    internal override Boolean __IsSignaled()
    {
        return __owner == 0 || __owner == Thread.CurrentThread.ManagedThreadId;
    }

    internal override Void __Consume()
    {
        __owner = Thread.CurrentThread.ManagedThreadId;
        __count = __count + 1;
    }
}
