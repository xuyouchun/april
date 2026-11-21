/*
 *  System.dll
 *
 *  Stopwatch: measures a length of time.
 */

////////// ////////// ////////// ////////// //////////

namespace Diagnostics;

// Measures the time with a clock that only goes on (not the clock of the system, that can be set). The ticks of the
// stopwatch are those of TimeSpan: 100 nanoseconds. A stopwatch is not safe to be used by many threads at once.
public class Stopwatch
{
    public Stopwatch()
    {
        __elapsed = 0;
        __started = 0;
        __running = false;
    }

    // The ticks in a second.
    public static Int64 Frequency => TimeSpan.TicksPerSecond;

    public static Boolean IsHighResolution => true;

    // The time of the clock, in ticks: the difference of two is a length of time.
    public static Int64 GetTimestamp() => Internal.Time_MonotonicTicks();

    // A stopwatch that is started.
    public static Stopwatch StartNew()
    {
        Stopwatch watch = new Stopwatch();
        watch.Start();
        return watch;
    }

    public Boolean IsRunning => __running;

    // Starts, or goes on after Stop. Nothing is done when it runs.
    public Void Start()
    {
        if (__running)
            return;

        __started = GetTimestamp();
        __running = true;
    }

    // Stops: the time is kept.
    public Void Stop()
    {
        if (!__running)
            return;

        __elapsed += GetTimestamp() - __started;
        __running = false;
    }

    // Stops and sets the time to zero.
    public Void Reset()
    {
        __elapsed = 0;
        __running = false;
    }

    // Sets the time to zero and starts.
    public Void Restart()
    {
        __elapsed = 0;
        __started = GetTimestamp();
        __running = true;
    }

    public Int64 ElapsedTicks => __running ? __elapsed + (GetTimestamp() - __started) : __elapsed;

    public Int64 ElapsedMilliseconds => ElapsedTicks / TimeSpan.TicksPerMillisecond;

    public TimeSpan Elapsed => new TimeSpan(ElapsedTicks);

    private Int64 __elapsed;
    private Int64 __started;
    private Boolean __running;
}
