// lock (obj) body: Monitor.Enter(obj); try { body } finally { Monitor.Exit(obj); }: it is held again by the same
// thread (a count), released by return, continue, and an exception. volatile is accepted on fields.

/* EXPECT

2 5 False 1 0
caught False
i0
i2
True

*/

import System;

using System;
using System.Threading;

class Counter
{
    private volatile Int32 _n;
    public static volatile Boolean Flag;
    private Object _gate = new Object();

    public Int32 Add(Int32 k)
    {
        lock (_gate)
        {
            _n += k;
            lock (_gate) { _n += 0; }
            return _n;
        }
    }

    public Int32 Early(Int32 k)
    {
        lock (this) { if (k > 0) return 1; }
        return 0;
    }

    public Boolean Held() { return Monitor.IsEntered(_gate); }

    public Void Boom() { lock (_gate) { throw new InvalidOperationException("boom"); } }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Counter c = new Counter();
        Console.WriteLine("" + c.Add(2) + " " + c.Add(3) + " " + c.Held() + " " + c.Early(1) + " " + c.Early(0));

        try { c.Boom(); } catch (InvalidOperationException e) { Console.WriteLine("caught " + c.Held()); }

        for (Int32 i = 0; i < 3; i++)
        {
            lock (c)
            {
                if (i == 1)
                    continue;

                Console.WriteLine("i" + i);
            }
        }

        Counter.Flag = true;
        Console.WriteLine(Counter.Flag);
    }
}
