// Monitor: the lock statement across threads, TryEnter (with a time), Wait and Pulse (a producer and a consumer),
// PulseAll, the lock that is taken again, and the errors.

/* EXPECT

sum 15
pulseall 3
timeout False True
reenter True
errors 3

*/

import System;

using System;
using System.Threading;

class Queue
{
    public Int32 Value;
    public Boolean Ready;
    public Int32 Sum;
    public Int32 Woken;
    public Boolean Go;
}

class Program
{
    static Queue __q = new Queue();

    static Void Produce()
    {
        for (Int32 i = 1; i <= 5; i++)
        {
            lock (__q)
            {
                while (__q.Ready)
                    Monitor.Wait(__q);

                __q.Value = i;
                __q.Ready = true;
                Monitor.PulseAll(__q);
            }
        }
    }

    static Void Consume()
    {
        for (Int32 i = 1; i <= 5; i++)
        {
            lock (__q)
            {
                while (!__q.Ready)
                    Monitor.Wait(__q);

                __q.Sum = __q.Sum + __q.Value;
                __q.Ready = false;
                Monitor.PulseAll(__q);
            }
        }
    }

    static Void WaitForGo()
    {
        lock (__q)
        {
            while (!__q.Go)
                Monitor.Wait(__q);

            __q.Woken++;
        }
    }

    static Void Hold(Object gate)
    {
        lock (gate) { Thread.Sleep(200); }
    }

    [EntryPoint]
    public static void Main()
    {
        Thread producer = new Thread(Produce), consumer = new Thread(Consume);
        consumer.Start(); producer.Start();
        producer.Join(); consumer.Join();
        Console.WriteLine("sum " + __q.Sum);

        Thread w1 = new Thread(WaitForGo), w2 = new Thread(WaitForGo), w3 = new Thread(WaitForGo);
        w1.Start(); w2.Start(); w3.Start();
        Thread.Sleep(50);
        lock (__q) { __q.Go = true; Monitor.PulseAll(__q); }
        w1.Join(); w2.Join(); w3.Join();
        Console.WriteLine("pulseall " + __q.Woken);

        // A lock that another thread holds: TryEnter fails until the time is over, then it is taken.
        Object gate = new Object();
        Delegate<Void, Object> hold = Hold;
        Thread holder = new Thread(hold);
        holder.Start(gate);
        Thread.Sleep(50);
        Boolean quick = Monitor.TryEnter(gate, 10);
        Boolean later = Monitor.TryEnter(gate, 10000);
        if (later)
            Monitor.Exit(gate);

        holder.Join();
        Console.WriteLine("timeout " + quick + " " + later);

        // The lock is taken again by the thread that holds it, and Wait gives all of them back.
        Object o = new Object();
        lock (o) { lock (o) { Console.WriteLine("reenter " + Monitor.IsEntered(o)); } }

        Int32 errors = 0;
        try { Monitor.Exit(o); } catch (SynchronizationLockException e) { errors++; }
        try { Monitor.Wait(o); } catch (SynchronizationLockException e) { errors++; }
        try { Monitor.Pulse(o); } catch (SynchronizationLockException e) { errors++; }
        Console.WriteLine("errors " + errors);
    }
}
