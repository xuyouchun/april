// Parallel: For, ForEach and Invoke run in the pool and end when all the iterations did; an exception stops the loop and is
// thrown in an AggregateException.

/* EXPECT

for 499500
foreach 55 10
invoke 3
error True
empty 0

*/

import System;

using System;
using System.Collections;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static Int64 __sum;
    static Int32 __items;
    static Int32 __invoked;
    static Int32 __ran;

    static Void Add(Int32 i) { Interlocked.Add(ref __sum, i); }

    static Void Count(Int32 item) { Interlocked.Add(ref __items, item); }

    static Void Hit() { Interlocked.Increment(ref __invoked); }

    static Void Fail(Int32 i)
    {
        Interlocked.Increment(ref __ran);

        if (i == 5)
            throw new InvalidOperationException("five");
    }

    [EntryPoint]
    public static void Main()
    {
        Parallel.For(0, 1000, Add);
        Console.WriteLine("for " + __sum);

        List<Int32> list = new List<Int32>();
        for (Int32 i = 1; i <= 10; i++)
            list.Add(i);

        Parallel.ForEach<Int32>(list, Count);
        Console.WriteLine("foreach " + __items + " " + list.Count);

        Delegate<Void>[] actions = new Delegate<Void>[3];
        actions[0] = Hit;
        actions[1] = Hit;
        actions[2] = Hit;
        Parallel.Invoke(actions);
        Console.WriteLine("invoke " + __invoked);

        Boolean error = false;
        try { Parallel.For(0, 50, Fail); } catch (AggregateException e) { error = e.InnerExceptions.Length >= 1; }
        Console.WriteLine("error " + error);

        __sum = 0;
        Parallel.For(5, 5, Add);
        Console.WriteLine("empty " + __sum);
    }
}
