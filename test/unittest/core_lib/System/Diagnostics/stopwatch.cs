// Stopwatch and Environment.TickCount: the clock that only goes on. The values of the time change from a run to another, so
// the program tells what is true of them: they are not negative, they do not go back, a stopwatch that is stopped keeps its
// time, a stopwatch that runs has gone on after a wait.

/* EXPECT

timestamp True 10000000 True
tick count True True True
new False 0 0 00:00:00
started True
stopped False True True True True
kept True
goes on True True
start again True True
stop again False
reset False 0
restart True True
after restart True True
start new True True True
monotonic 0
difference True True
done

*/

import System;

using System;
using System.Diagnostics;

class Program
{
    // Waits until the tick count has gone on by the number of milliseconds (a loop: the library has no Sleep yet). The tick count
    // is in whole milliseconds, so the real time is the number minus one millisecond at least: the checks below ask for less.
    static Void Wait(Int64 milliseconds)
    {
        Int64 start = Environment.TickCount64;
        while (Environment.TickCount64 - start < milliseconds)
        {
        }
    }

    [EntryPoint]
    public static void Main()
    {
        // The clock.
        Int64 t1 = Stopwatch.GetTimestamp();
        Int64 t2 = Stopwatch.GetTimestamp();
        Console.WriteLine("timestamp " + (t2 >= t1) + " " + Stopwatch.Frequency + " " + Stopwatch.IsHighResolution);

        Int64 k1 = Environment.TickCount64;
        Wait(35);
        Int64 k2 = Environment.TickCount64;
        Console.WriteLine("tick count " + (k2 - k1 >= 30) + " " + (k2 - k1 < 5000) + " " + (Environment.TickCount != 0 || Environment.TickCount64 >= 0));

        // The new stopwatch.
        Stopwatch w = new Stopwatch();
        Console.WriteLine("new " + w.IsRunning + " " + w.ElapsedTicks + " " + w.ElapsedMilliseconds + " " + w.Elapsed);

        // Start: the time goes on; Stop: it is kept.
        w.Start();
        Console.WriteLine("started " + w.IsRunning);
        Wait(35);
        w.Stop();
        Int64 first = w.ElapsedTicks;
        Console.WriteLine("stopped " + w.IsRunning + " " + (first >= 30 * TimeSpan.TicksPerMillisecond) + " " + (w.ElapsedMilliseconds >= 30) + " " + (w.Elapsed.TotalMilliseconds >= 30.0) + " " + (w.Elapsed.Ticks == first));

        Wait(25);
        Console.WriteLine("kept " + (w.ElapsedTicks == first));

        // Start goes on after Stop: the time is added.
        w.Start();
        Wait(35);
        Int64 second = w.ElapsedTicks;
        Console.WriteLine("goes on " + (second >= first + 30 * TimeSpan.TicksPerMillisecond) + " " + w.IsRunning);
        w.Start();
        Console.WriteLine("start again " + w.IsRunning + " " + (w.ElapsedTicks >= second));
        w.Stop();
        w.Stop();
        Console.WriteLine("stop again " + w.IsRunning);

        // Reset: zero and stopped. Restart: zero and running.
        w.Reset();
        Console.WriteLine("reset " + w.IsRunning + " " + w.ElapsedTicks);
        w.Start();
        Wait(60);
        w.Restart();
        Console.WriteLine("restart " + w.IsRunning + " " + (w.ElapsedMilliseconds < 60));
        Wait(25);
        Console.WriteLine("after restart " + (w.ElapsedMilliseconds >= 20) + " " + (w.ElapsedMilliseconds < 5000));

        // StartNew.
        Stopwatch n = Stopwatch.StartNew();
        Wait(15);
        Console.WriteLine("start new " + n.IsRunning + " " + (n.ElapsedMilliseconds >= 10) + " " + (n.Elapsed >= TimeSpan.FromMilliseconds(10)));

        // The elapsed time never goes back.
        Stopwatch m = Stopwatch.StartNew();
        Int64 last = 0;
        Int32 back = 0;
        for (Int32 i = 0; i < 10000; i++)
        {
            Int64 now = m.ElapsedTicks;
            if (now < last)
                back++;

            last = now;
        }

        Console.WriteLine("monotonic " + back);

        // The same thing with the ticks of the clock and the difference of two times.
        Int64 a = Stopwatch.GetTimestamp();
        Wait(20);
        Int64 b = Stopwatch.GetTimestamp();
        Console.WriteLine("difference " + (TimeSpan.FromTicks(b - a) >= TimeSpan.FromMilliseconds(15)) + " " + (Environment.NewLine == "\n"));

        Console.WriteLine("done");
    }
}
