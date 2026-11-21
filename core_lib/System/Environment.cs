/*
 *  System.dll
 *
 *  Environment: what the system gives to the program.
 */

////////// ////////// ////////// ////////// //////////

// Information of the system. For now only the time since the start, and the new line.
public static class Environment
{
    // The milliseconds that the clock that only goes on has counted (its zero is not defined: use the differences).
    public static Int64 TickCount64 => Internal.Time_MonotonicTicks() / TimeSpan.TicksPerMillisecond;

    // The same, in an Int32 that goes round to the negative numbers after 24.9 days, as in .NET.
    public static Int32 TickCount => (Int32)(TickCount64 & 0xFFFFFFFF);

    // The end of a line.
    public static String NewLine => "\n";
}
