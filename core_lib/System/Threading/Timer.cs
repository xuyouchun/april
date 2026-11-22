/*
 *  System.dll
 *
 *  Timer: a method that runs after a time, and again and again.
 */

using Collections;

namespace Threading;

////////// ////////// ////////// ////////// //////////

// Calls a method of the thread pool after a time (the due time), then each period if there is one. One background thread
// watches the timers. Timeout.Infinite for the due time makes the timer wait (it runs when Change sets a time), for the period
// it makes it run once. A method that is late is called once, not for each period that was missed.
public sealed class Timer : IDisposable
{
    private Delegate<Void, Object> __callback;
    private ExecutionContext? __context = ExecutionContext.Capture();
    private Object? __state;
    private Int64 __next;               // The time of the next call (milliseconds of the clock that only goes on), -1 when none.
    private Int32 __period;             // Milliseconds, -1 for none.
    private Boolean __disposed;

    // The timers that wait, and the thread that watches them.
    private static Object __gate = new Object();
    private static List<Timer> __timers = new List<Timer>();
    private static Boolean __watching;

    public Timer(Delegate<Void, Object> callback, Object? state, Int32 dueTime, Int32 period)
    {
        __callback = callback;
        __state = state;
        __period = -1;
        __next = -1;

        __Set(dueTime, period);
    }

    public Timer(Delegate<Void, Object> callback, Object? state, TimeSpan dueTime, TimeSpan period)
        : this(callback, state, Waiting.ToMilliseconds(dueTime), Waiting.ToMilliseconds(period))
    {
    }

    // Changes the time of the next call and the period, returns false when the timer was disposed.
    public Boolean Change(Int32 dueTime, Int32 period)
    {
        if (__disposed)
            return false;

        __Set(dueTime, period);
        return true;
    }

    public Boolean Change(TimeSpan dueTime, TimeSpan period)
    {
        return Change(Waiting.ToMilliseconds(dueTime), Waiting.ToMilliseconds(period));
    }

    // The timer does not call its method any more.
    public Void Dispose()
    {
        lock (__gate)
        {
            __disposed = true;
            __next = -1;
            __timers.Remove(this);

            Monitor.PulseAll(__gate);
        }
    }

    private Void __Set(Int32 dueTime, Int32 period)
    {
        Waiting.Check(dueTime);
        Waiting.Check(period);

        Boolean start = false;

        lock (__gate)
        {
            __period = period;
            __next = dueTime < 0? -1 : Environment.TickCount64 + dueTime;

            if (!__timers.Contains(this))
                __timers.Add(this);

            if (!__watching)
            {
                __watching = true;
                start = true;
            }

            Monitor.PulseAll(__gate);
        }

        if (start)
        {
            Thread watcher = new Thread(__Watch);
            watcher.IsBackground = true;
            watcher.Start();
        }
    }

    // The thread that waits for the next time of a timer, and queues the call.
    private static Void __Watch()
    {
        for (;;)
        {
            List<Timer> due = new List<Timer>();

            lock (__gate)
            {
                Int64 now = Environment.TickCount64;
                Int64 earliest = -1;

                for (Int32 k = 0; k < __timers.Count; k++)
                {
                    Timer timer = __timers[k];
                    if (timer.__next < 0)
                        continue;

                    if (timer.__next <= now)
                    {
                        due.Add(timer);

                        // The next time: after the period, or never.
                        timer.__next = timer.__period > 0? now + timer.__period : -1;
                    }

                    if (timer.__next >= 0 && (earliest < 0 || timer.__next < earliest))
                        earliest = timer.__next;
                }

                if (due.Count == 0)
                {
                    if (earliest < 0)
                        Monitor.Wait(__gate);
                    else
                        Monitor.Wait(__gate, (Int32)(earliest - now > 2147483647? 2147483647 : earliest - now));

                    continue;
                }
            }

            // The call has the context of the code that made the timer (the pool keeps the one that is queuing).
            for (Int32 k = 0; k < due.Count; k++)
            {
                ExecutionContext? saved = ExecutionContext.__Swap(due[k].__context);
                ThreadPool.QueueUserWorkItem(due[k].__callback, due[k].__state);
                ExecutionContext.__Swap(saved);
            }
        }
    }
}
