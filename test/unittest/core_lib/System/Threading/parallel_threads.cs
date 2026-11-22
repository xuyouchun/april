// The code of the threads runs in parallel in the native programs (Thread.IsParallel): four threads that compute take about the
// time of one; the threads that allocate, collect and use Interlocked and locks give the right results (the interpreter runs
// one thread at a time: the results are the same).

/* EXPECT

scale True
alloc 483093600 6
counter 600000 600000 True

*/

import System;

using System;
using System.Threading;

class Node
{
    public Int32 V;
    public Node? Next;
}

class Program
{
    static Int64[] __sums = new Int64[8];
    static Int64[] __values = new Int64[8];
    static Int32 __atomic;
    static Int32 __locked;
    static Object __gate = new Object();

    static Int64 Spin(Int32 n)
    {
        Int64 x = 0;
        for (Int32 i = 0; i < n; i++)
            x = x + (i % 7) * (i % 11);

        return x;
    }

    static Void Compute(Object o)
    {
        Int32 k = (Int32)o;
        __values[k] = Spin(30000000);
    }

    // (Few rounds: a program that collects at each allocation, APRIL_GC_STRESS, stops and starts all the threads for each one, that takes
    // some milliseconds: the test has the same number of threads and objects, and runs in a minute then.)
    static Void Allocate(Object o)
    {
        Int32 id = (Int32)o;
        Int64 sum = 0;

        for (Int32 round = 0; round < 4; round++)
        {
            Node? head = null;
            for (Int32 i = 0; i < 2000; i++)
            {
                Node n = new Node();
                n.V = i + id;
                n.Next = head;
                head = n;

                String s = "k" + i;
                sum += s.Length;
            }

            Node? p = head;
            while (p != null)
            {
                sum += p.V;
                p = p.Next;
            }
        }

        __sums[id] = sum * 10;
    }

    static Void Count()
    {
        for (Int32 i = 0; i < 100000; i++)
        {
            Interlocked.Increment(ref __atomic);

            lock (__gate)
                __locked++;
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Int64 start = Environment.TickCount64;
        __values[0] = Spin(30000000);
        Int64 one = Environment.TickCount64 - start;

        Delegate<Void, Object> compute = Compute;
        Thread[] threads = new Thread[4];

        start = Environment.TickCount64;
        for (Int32 i = 0; i < 4; i++)
        {
            threads[i] = new Thread(compute);
            threads[i].Start(i);
        }

        for (Int32 i = 0; i < 4; i++)
            threads[i].Join();

        Int64 four = Environment.TickCount64 - start;

        // Parallel: four threads take less than 2.5 times what one takes (not measured with one processor or in the interpreter).
        Boolean fast = !Thread.IsParallel || Environment.ProcessorCount < 4 || four < one * 5 / 2 + 50;
        Console.WriteLine("scale " + (fast && __values[0] == __values[3]));

        Delegate<Void, Object> allocate = Allocate;
        Thread[] allocators = new Thread[6];
        for (Int32 i = 0; i < 6; i++)
        {
            allocators[i] = new Thread(allocate);
            allocators[i].Start(i);
        }

        for (Int32 i = 0; i < 6; i++)
            allocators[i].Join();

        Int64 total = 0;
        for (Int32 i = 0; i < 6; i++)
            total += __sums[i];

        Console.WriteLine("alloc " + total + " 6");

        Thread[] counters = new Thread[6];
        for (Int32 i = 0; i < 6; i++)
        {
            counters[i] = new Thread(Count);
            counters[i].Start();
        }

        for (Int32 i = 0; i < 6; i++)
            counters[i].Join();

        Console.WriteLine("counter " + __atomic + " " + __locked + " " + (__atomic == __locked));
    }
}
