/*
 *  System.dll
 *
 *  Monitor: the lock of an object, the lock statement.
 */

using Collections;

namespace Threading;

////////// ////////// ////////// ////////// //////////

// Exit is called by a thread that does not hold the lock of the object.
public class SynchronizationLockException : SystemException
{
    public SynchronizationLockException() { }

    public SynchronizationLockException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// The lock of an object: lock (obj) { ... } is Monitor.Enter(obj); try { ... } finally { Monitor.Exit(obj); }.
// One managed thread runs at a time, so a lock can only be held again by the thread that holds it (it counts how
// many times); the threads that wait for it come with System.Threading.Thread.
public static class Monitor
{
    private static Dictionary<Object, Int32>? __table;

    // The locks that are held, and how many times each (made when it is first needed).
    private static Dictionary<Object, Int32> __held
    {
        get
        {
            if (__table == null)
                __table = new Dictionary<Object, Int32>();

            return __table;
        }
    }

    // Takes the lock of the object, waits until it is free.
    public static Void Enter(Object obj)
    {
        if (obj == null)
            throw new ArgumentException("the object of a lock is null");

        Int32 count;
        __held.TryGetValue(obj, out count);
        __held[obj] = count + 1;
    }

    // Takes the lock if it is free (or held by this thread), returns whether it was taken.
    public static Boolean TryEnter(Object obj)
    {
        Enter(obj);
        return true;
    }

    // Releases the lock of the object.
    public static Void Exit(Object obj)
    {
        if (obj == null)
            throw new ArgumentException("the object of a lock is null");

        Int32 count;
        if (!__held.TryGetValue(obj, out count) || count == 0)
            throw new SynchronizationLockException("the lock is not held");

        if (count == 1)
            __held.Remove(obj);
        else
            __held[obj] = count - 1;
    }

    // Returns whether the lock of the object is held.
    public static Boolean IsEntered(Object obj)
    {
        Int32 count;
        return obj != null && __held.TryGetValue(obj, out count) && count > 0;
    }
}
