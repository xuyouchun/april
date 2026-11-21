/*
 *  System.dll
 *
 *  Volatile: reads and writes that the other threads see in order; Timeout.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// One thread runs at a time, every read sees the last write: the methods read and write.
public static class Volatile
{
    public static T Read<T>(ref T location)
    {
        return location;
    }

    public static Void Write<T>(ref T location, T value)
    {
        location = value;
    }
}

////////// ////////// ////////// ////////// //////////

// Times of waiting.
public static class Timeout
{
    // The time that has no end.
    public const Int32 Infinite = -1;

    public static TimeSpan InfiniteTimeSpan => TimeSpan.FromMilliseconds(-1.0);
}
