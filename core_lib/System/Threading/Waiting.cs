/*
 *  System.dll
 *
 *  The times of the operations that wait.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A wait has a time in milliseconds (-1 is for ever): the end is a time of the clock that only goes on, and what is left is
// asked each time that the wait goes on.
internal static class Waiting
{
    // The time that a wait of some milliseconds ends, 0 for ever.
    internal static Int64 End(Int32 milliseconds)
    {
        if (milliseconds < 0)
            return 0;

        return Environment.TickCount64 + milliseconds;
    }

    // The milliseconds that are left: -1 for ever, 0 when the time is over.
    internal static Int32 Left(Int64 end, Int32 milliseconds)
    {
        if (milliseconds < 0)
            return -1;

        Int64 left = end - Environment.TickCount64;
        if (left <= 0)
            return 0;

        return left > 2147483647? 2147483647 : (Int32)left;
    }

    // Checks the time of a call: -1 or more.
    internal static Void Check(Int32 milliseconds)
    {
        if (milliseconds < -1)
            throw new ArgumentException("the time is negative");
    }

    // The milliseconds of a time span.
    internal static Int32 ToMilliseconds(TimeSpan timeout)
    {
        Double milliseconds = timeout.TotalMilliseconds;

        if (milliseconds < -1.0 || milliseconds > 2147483647.0)
            throw new ArgumentException("the time is out of range");

        return (Int32)milliseconds;
    }
}
