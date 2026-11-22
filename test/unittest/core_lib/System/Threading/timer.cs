// Timer: a call after the due time, the periodic calls, Change, Dispose, and CancelAfter of a cancellation source.

/* EXPECT

once 1
periodic True
changed True
disposed True
cancel True

*/

import System;

using System;
using System.Threading;

class Program
{
    static Int32 __once;
    static Int32 __periodic;
    static Int32 __changed;
    static ManualResetEventSlim __fired = new ManualResetEventSlim(false);

    static Void Once(Object state) { Interlocked.Increment(ref __once); __fired.Set(); }

    static Void Periodic(Object state) { Interlocked.Increment(ref __periodic); }

    static Void Changed(Object state) { Interlocked.Increment(ref __changed); }

    [EntryPoint]
    public static void Main()
    {
        Delegate<Void, Object> once = Once, periodic = Periodic, changed = Changed;

        Timer a = new Timer(once, null, 30, Timeout.Infinite);
        __fired.Wait(5000);
        Thread.Sleep(100);
        Console.WriteLine("once " + __once);

        Timer b = new Timer(periodic, null, 10, 20);
        Thread.Sleep(300);
        Console.WriteLine("periodic " + (__periodic >= 4));

        // Not started: Change sets its time.
        Timer c = new Timer(changed, null, Timeout.Infinite, Timeout.Infinite);
        Thread.Sleep(60);
        Int32 before = __changed;
        c.Change(10, Timeout.Infinite);
        Thread.Sleep(150);
        Console.WriteLine("changed " + (before == 0 && __changed == 1));

        b.Dispose();
        Int32 atDispose = __periodic;
        Thread.Sleep(120);
        Console.WriteLine("disposed " + (__periodic <= atDispose + 1 && !b.Change(1, 1)));

        CancellationTokenSource source = new CancellationTokenSource();
        source.CancelAfter(40);
        Boolean early = source.IsCancellationRequested;
        Thread.Sleep(200);
        Console.WriteLine("cancel " + (!early && source.IsCancellationRequested));

        a.Dispose();
        c.Dispose();
    }
}
