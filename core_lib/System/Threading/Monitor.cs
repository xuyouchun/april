/*
 *  System.dll
 *
 *  Monitor: the lock of an object, the lock statement.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A method of Monitor is called by a thread that does not hold the lock of the object.
public class SynchronizationLockException : SystemException
{
    public SynchronizationLockException() { }

    public SynchronizationLockException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// The lock of an object: lock (obj) { ... } is Monitor.Enter(obj); try { ... } finally { Monitor.Exit(obj); }.
// A thread can take the lock of an object again (it counts how many times); the other threads wait until the thread has
// released it as many times. Wait and Pulse are for the threads that wait for a change of the state that the lock guards.
public static class Monitor
{
    // Takes the lock of the object, waits until it is free.
    public static Void Enter(Object obj)
    {
        __CheckObject(obj);
        Internal.Monitor_Enter(obj);
    }

    // Takes the lock if it is free (or held by this thread), returns whether it was taken.
    public static Boolean TryEnter(Object obj)
    {
        __CheckObject(obj);
        return Internal.Monitor_TryEnter(obj, 0);
    }

    // Takes the lock if it is free before the time is over (milliseconds, -1 is for ever), returns whether it was taken.
    public static Boolean TryEnter(Object obj, Int32 millisecondsTimeout)
    {
        __CheckObject(obj);
        __CheckTimeout(millisecondsTimeout);

        return Internal.Monitor_TryEnter(obj, millisecondsTimeout);
    }

    public static Boolean TryEnter(Object obj, TimeSpan timeout)
    {
        __CheckObject(obj);
        return Internal.Monitor_TryEnter(obj, __ToMilliseconds(timeout));
    }

    // Releases the lock of the object, throws SynchronizationLockException when the thread does not hold it.
    public static Void Exit(Object obj)
    {
        __CheckObject(obj);

        if (Internal.Monitor_Exit(obj) != 0)
            throw new SynchronizationLockException("the lock of the object is not held by this thread");
    }

    // Returns whether the thread holds the lock of the object.
    public static Boolean IsEntered(Object obj)
    {
        __CheckObject(obj);
        return Internal.Monitor_IsEntered(obj);
    }

    // Releases the lock of the object and waits for a Pulse (or PulseAll) of another thread, then takes the lock again.
    // Returns false when the time was over (milliseconds, -1 is for ever), the lock is taken in both cases.
    public static Boolean Wait(Object obj, Int32 millisecondsTimeout)
    {
        __CheckObject(obj);
        __CheckTimeout(millisecondsTimeout);

        Int32 result = Internal.Monitor_Wait(obj, millisecondsTimeout);
        if (result == 0)
            throw new SynchronizationLockException("the lock of the object is not held by this thread");

        return result == 1;
    }

    public static Boolean Wait(Object obj) => Wait(obj, -1);

    public static Boolean Wait(Object obj, TimeSpan timeout) => Wait(obj, __ToMilliseconds(timeout));

    // Wakes up the thread that has waited the longest in the object.
    public static Void Pulse(Object obj)
    {
        __CheckObject(obj);

        if (!Internal.Monitor_Pulse(obj, false))
            throw new SynchronizationLockException("the lock of the object is not held by this thread");
    }

    // Wakes up all the threads that wait in the object.
    public static Void PulseAll(Object obj)
    {
        __CheckObject(obj);

        if (!Internal.Monitor_Pulse(obj, true))
            throw new SynchronizationLockException("the lock of the object is not held by this thread");
    }

    private static Void __CheckObject(Object? obj)
    {
        if (obj == null)
            throw new ArgumentException("the object of a lock is null");
    }

    private static Void __CheckTimeout(Int32 milliseconds)
    {
        if (milliseconds < -1)
            throw new ArgumentException("the time is negative");
    }

    private static Int32 __ToMilliseconds(TimeSpan timeout)
    {
        Double milliseconds = timeout.TotalMilliseconds;

        if (milliseconds < -1.0 || milliseconds > 2147483647.0)
            throw new ArgumentException("the time is out of range");

        return (Int32)milliseconds;
    }
}
