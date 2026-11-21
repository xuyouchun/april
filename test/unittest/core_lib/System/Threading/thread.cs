// Thread: Start, Join (with a time), the parameter, the ids, IsAlive, IsBackground, Sleep, Yield, the errors.

/* EXPECT

main 1 True
counted 3000
param hello
alive False False
join False True
ids True
background True
errors 4

*/

import System;

using System;
using System.Threading;

class Counter
{
    public Int32 Value;
    public Object Gate = new Object();
}

class Program
{
    static Counter __counter = new Counter();
    static String __param = "";
    static Boolean __ids = false;

    static Void Count()
    {
        for (Int32 i = 0; i < 1000; i++)
        {
            lock (__counter.Gate) { __counter.Value = __counter.Value + 1; }
            Thread.Yield();
        }
    }

    static Void WithParameter(Object o) { __param = "param " + o; }

    static Void Slow() { Thread.Sleep(300); }

    static Void SameId() { __ids = Thread.CurrentThread.ManagedThreadId > 1; }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("main " + Thread.CurrentThread.ManagedThreadId + " " + Object.ReferenceEquals(Thread.CurrentThread, Thread.CurrentThread));

        Thread a = new Thread(Count), b = new Thread(Count), c = new Thread(Count);
        a.Start(); b.Start(); c.Start();
        a.Join(); b.Join(); c.Join();
        Console.WriteLine("counted " + __counter.Value);

        Delegate<Void, Object> withParameter = WithParameter;
        Thread p = new Thread(withParameter);
        p.Start("hello");
        p.Join();
        Console.WriteLine(__param);

        Thread s = new Thread(Slow);
        Console.WriteLine("alive " + s.IsAlive + " " + (s.IsBackground));
        s.Start();
        Boolean early = s.Join(10);
        Console.WriteLine("join " + early + " " + s.Join(10000));

        Thread i = new Thread(SameId);
        i.Start(); i.Join();
        Console.WriteLine("ids " + __ids);

        Thread g = new Thread(Slow);
        g.IsBackground = true;
        g.Start();
        Console.WriteLine("background " + g.IsBackground);

        Int32 errors = 0;
        try { s.Start(); } catch (InvalidOperationException e) { errors++; }
        try { new Thread(Count).Join(); } catch (InvalidOperationException e) { errors++; }
        try { Thread.CurrentThread.Join(); } catch (InvalidOperationException e) { errors++; }
        try { Thread.Sleep(-5); } catch (ArgumentException e) { errors++; }
        Console.WriteLine("errors " + errors);
    }
}
