/*
 *  System.dll
 *
 *  Interlocked: operations on a variable that no other thread interrupts.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// The operations of numbers are atomic instructions of the machine (the code of the threads runs in parallel in the native
// programs); the ones of references take the lock of the class.
public static class Interlocked
{
    // Adds one, returns the new value.
    public static Int32 Increment(ref Int32 location)
    {
        return Internal.Interlocked_Add32(ref location, 1);
    }

    public static Int64 Increment(ref Int64 location)
    {
        return Internal.Interlocked_Add64(ref location, 1);
    }

    // Subtracts one, returns the new value.
    public static Int32 Decrement(ref Int32 location)
    {
        return Internal.Interlocked_Add32(ref location, -1);
    }

    public static Int64 Decrement(ref Int64 location)
    {
        return Internal.Interlocked_Add64(ref location, -1);
    }

    // Adds a value, returns the new value.
    public static Int32 Add(ref Int32 location, Int32 value)
    {
        return Internal.Interlocked_Add32(ref location, value);
    }

    public static Int64 Add(ref Int64 location, Int64 value)
    {
        return Internal.Interlocked_Add64(ref location, value);
    }

    // Sets a value, returns the one that was there.
    public static Int32 Exchange(ref Int32 location, Int32 value)
    {
        return Internal.Interlocked_Exchange32(ref location, value);
    }

    public static Int64 Exchange(ref Int64 location, Int64 value)
    {
        return Internal.Interlocked_Exchange64(ref location, value);
    }

    // Sets the value if the variable has the compared one, returns the one that was there.
    public static Int32 CompareExchange(ref Int32 location, Int32 value, Int32 comparand)
    {
        return Internal.Interlocked_CompareExchange32(ref location, value, comparand);
    }

    public static Int64 CompareExchange(ref Int64 location, Int64 value, Int64 comparand)
    {
        return Internal.Interlocked_CompareExchange64(ref location, value, comparand);
    }

    // Reads a 64 bits number.
    public static Int64 Read(ref Int64 location)
    {
        return Internal.Interlocked_CompareExchange64(ref location, 0, 0);
    }

    // Sets a reference, returns the one that was there.
    public static T Exchange<T>(ref T location, T value) where T : class
    {
        lock (typeof(Interlocked))
        {
            T old = location;
            location = value;
            return old;
        }
    }

    // Sets the reference if the variable is the compared one (the same object), returns the one that was there.
    public static T CompareExchange<T>(ref T location, T value, T comparand) where T : class
    {
        lock (typeof(Interlocked))
        {
            T old = location;
            if (Object.ReferenceEquals(old, comparand))
                location = value;

            return old;
        }
    }

    // A barrier of the memory: the threads see the operations in order, nothing to do.
    public static Void MemoryBarrier()
    {
    }
}
