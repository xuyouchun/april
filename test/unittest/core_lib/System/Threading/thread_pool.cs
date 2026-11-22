// ThreadPool: work with a state and without, many items, the work that waits for other work (the pool makes threads), the
// counts.

/* EXPECT

queued 3 100
sum 4950
blocked 8
threads True

*/

import System;

using System;
using System.Threading;

class Program
{
    static Int32 __count;
    static Int32 __sum;
    static ManualResetEvent __release = new ManualResetEvent(false);
    static CountdownEvent __arrived = new CountdownEvent(8);
    static Int32 __passed;

    static Void Plain() { Interlocked.Increment(ref __count); }

    static Void WithState(Object state) { Interlocked.Add(ref __count, (Int32)state); }

    static Void Add(Object state) { Interlocked.Add(ref __sum, (Int32)state); }

    // Waits for the others: it takes eight threads to go on.
    static Void Blocked()
    {
        __arrived.Signal();
        __release.WaitOne();
        Interlocked.Increment(ref __passed);
    }

    [EntryPoint]
    public static void Main()
    {
        Delegate<Void, Object> withState = WithState;
        ThreadPool.QueueUserWorkItem(Plain);
        ThreadPool.QueueUserWorkItem(withState, 98);
        ThreadPool.QueueUserWorkItem(Plain);

        while (__count < 100)
            Thread.Sleep(5);

        Console.WriteLine("queued 3 " + __count);

        Delegate<Void, Object> add = Add;
        for (Int32 i = 0; i < 100; i++)
            ThreadPool.QueueUserWorkItem(add, i);

        while (ThreadPool.CompletedWorkItemCount < 103)
            Thread.Sleep(5);

        Console.WriteLine("sum " + __sum);

        // Eight works that wait for each other: the pool has to run them all at once.
        for (Int32 i = 0; i < 8; i++)
            ThreadPool.QueueUserWorkItem(Blocked);

        Boolean all = __arrived.Wait(10000);
        __release.Set();

        while (__passed < 8)
            Thread.Sleep(5);

        Console.WriteLine("blocked " + (all? __passed : -1));
        Console.WriteLine("threads " + (ThreadPool.ThreadCount >= 8));
    }
}
