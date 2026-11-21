/*
 *  System.dll
 *
 *  Interlocked: operations on a variable that no other thread interrupts.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// The threads run one at a time and switch only at the calls that wait and at the jumps that go back, so the operations
// of numbers (that have neither a call nor a loop) are not interrupted. The ones of references take the lock of the class.
public static class Interlocked
{
    // Adds one, returns the new value.
    public static Int32 Increment(ref Int32 location)
    {
        location = location + 1;
        return location;
    }

    public static Int64 Increment(ref Int64 location)
    {
        location = location + 1;
        return location;
    }

    // Subtracts one, returns the new value.
    public static Int32 Decrement(ref Int32 location)
    {
        location = location - 1;
        return location;
    }

    public static Int64 Decrement(ref Int64 location)
    {
        location = location - 1;
        return location;
    }

    // Adds a value, returns the new value.
    public static Int32 Add(ref Int32 location, Int32 value)
    {
        location = location + value;
        return location;
    }

    public static Int64 Add(ref Int64 location, Int64 value)
    {
        location = location + value;
        return location;
    }

    // Sets a value, returns the one that was there.
    public static Int32 Exchange(ref Int32 location, Int32 value)
    {
        Int32 old = location;
        location = value;
        return old;
    }

    public static Int64 Exchange(ref Int64 location, Int64 value)
    {
        Int64 old = location;
        location = value;
        return old;
    }

    // Sets the value if the variable has the compared one, returns the one that was there.
    public static Int32 CompareExchange(ref Int32 location, Int32 value, Int32 comparand)
    {
        Int32 old = location;
        if (old == comparand)
            location = value;

        return old;
    }

    public static Int64 CompareExchange(ref Int64 location, Int64 value, Int64 comparand)
    {
        Int64 old = location;
        if (old == comparand)
            location = value;

        return old;
    }

    // Reads a 64 bits number.
    public static Int64 Read(ref Int64 location)
    {
        return location;
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
