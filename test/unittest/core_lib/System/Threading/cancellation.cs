// CancellationTokenSource and CancellationToken: Cancel, the registered methods (in order, at once when it is canceled already,
// not after Dispose of the registration), ThrowIfCancellationRequested, the linked sources, a thread that stops.

/* EXPECT

before False True
callbacks 1 2
late 3
disposed 4
throw True
linked False True True
worker stopped True
none False False

*/

import System;

using System;
using System.Text;
using System.Threading;

class Program
{
    static StringBuilder __log = new StringBuilder();
    static Int32 __steps;
    static Int32 __stopped;

    static Void First() { __log.Append("1"); }

    static Void Second() { __log.Append("2"); }

    static Void Third() { __log.Append("3"); }

    static Void Fourth() { __log.Append("4"); }

    static Void Worker(Object o)
    {
        CancellationToken token = (CancellationToken)o;

        while (!token.IsCancellationRequested)
        {
            Interlocked.Increment(ref __steps);
            Thread.Yield();
        }

        __stopped = 1;
    }

    [EntryPoint]
    public static void Main()
    {
        CancellationTokenSource source = new CancellationTokenSource();
        CancellationToken token = source.Token;
        Console.WriteLine("before " + token.IsCancellationRequested + " " + token.CanBeCanceled);

        token.Register(First);
        CancellationTokenRegistration skipped = token.Register(Second);
        token.Register(Second);
        skipped.Dispose();
        source.Cancel();
        source.Cancel();
        Console.WriteLine("callbacks " + (__log.ToString() == "12"? 1 : 0) + " " + __log.Length);

        token.Register(Third);
        Console.WriteLine("late " + (__log.ToString() == "123"? 3 : 0));

        CancellationTokenRegistration r = token.Register(Fourth);
        r.Dispose();
        Console.WriteLine("disposed " + (__log.ToString() == "1234"? 4 : 0));

        Boolean thrown = false;
        try { token.ThrowIfCancellationRequested(); } catch (OperationCanceledException e) { thrown = true; }
        Console.WriteLine("throw " + thrown);

        CancellationTokenSource a = new CancellationTokenSource(), b = new CancellationTokenSource();
        CancellationTokenSource linked = CancellationTokenSource.CreateLinkedTokenSource(a.Token, b.Token);
        Boolean first = linked.IsCancellationRequested;
        b.Cancel();
        Console.WriteLine("linked " + first + " " + linked.IsCancellationRequested + " " + linked.Token.IsCancellationRequested);

        CancellationTokenSource stop = new CancellationTokenSource();
        Delegate<Void, Object> worker = Worker;
        Thread t = new Thread(worker);
        t.Start(stop.Token);
        Thread.Sleep(30);
        stop.Cancel();
        t.Join();
        Console.WriteLine("worker stopped " + (__stopped == 1 && __steps > 0));

        CancellationToken none = CancellationToken.None;
        Console.WriteLine("none " + none.IsCancellationRequested + " " + none.CanBeCanceled);
    }
}
