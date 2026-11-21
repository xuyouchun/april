// ThreadLocal<T>: a value for each thread (made by the factory the first time), IsValueCreated, Lazy<T> (once, with many threads,
// and an exception of the factory).

/* EXPECT

main False
values 1 2 4
created True
lazy 1 True 5
lazy threads 1
lazy error caught 2

*/

import System;

using System;
using System.Threading;

class Program
{
    static ThreadLocal<Int32> __local = new ThreadLocal<Int32>(Make);
    static Int32 __made;
    static Int32 __made_lazy;
    static Int32[] __seen = new Int32[3];
    static Lazy<Int32>? __lazy;
    static Int32 __factoryCalls;

    static Int32 Make()
    {
        return Interlocked.Increment(ref __made);
    }

    static Int32 MakeLazy()
    {
        Interlocked.Increment(ref __made_lazy);
        Thread.Sleep(30);
        return 5;
    }

    static Int32 Failing()
    {
        __factoryCalls++;
        if (__factoryCalls == 1)
            throw new InvalidOperationException("first");

        return __factoryCalls;
    }

    static Void Use(Object o)
    {
        Int32 index = (Int32)o;
        __seen[index] = __local.Value + __local.Value - __local.Value;
    }

    static Void UseLazy()
    {
        Int32 x = __lazy.Value;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("main " + __local.IsValueCreated);

        Delegate<Void, Object> use = Use;
        Thread[] threads = new Thread[2];
        for (Int32 i = 0; i < 2; i++)
        {
            threads[i] = new Thread(use);
            threads[i].Start(i);
            threads[i].Join();
        }

        Int32 mine = __local.Value;
        Console.WriteLine("values " + __seen[0] + " " + __seen[1] + " " + (mine + 1));
        Console.WriteLine("created " + __local.IsValueCreated);

        Lazy<Int32> lazy = new Lazy<Int32>(MakeLazy);
        Boolean before = lazy.IsValueCreated;
        Int32 value = lazy.Value + lazy.Value - 5;
        Console.WriteLine("lazy " + __made_lazy + " " + lazy.IsValueCreated + " " + value);

        __made_lazy = 0;
        __lazy = new Lazy<Int32>(MakeLazy);
        Thread[] readers = new Thread[4];
        for (Int32 i = 0; i < 4; i++)
        {
            readers[i] = new Thread(UseLazy);
            readers[i].Start();
        }

        for (Int32 i = 0; i < 4; i++)
            readers[i].Join();

        Console.WriteLine("lazy threads " + __made_lazy);

        Lazy<Int32> failing = new Lazy<Int32>(Failing);
        Boolean caught = false;
        try { Int32 x = failing.Value; } catch (InvalidOperationException e) { caught = true; }
        Console.WriteLine("lazy error " + (caught? "caught" : "no") + " " + failing.Value);
    }
}
