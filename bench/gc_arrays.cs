// Benchmark of the garbage collector: large and medium arrays that die at once.
// Run with APRIL_GC_STATS=1 (pauses) or APRIL_GC_TRACE=1 (a line per collection); APRIL_GC=0 turns the collector off.

import System;
using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int64 total = 0;
        for (Int32 i = 0; i < 3000; i++)
        {
            Int32[] a = new Int32[50000];
            a[i] = i;
            total += a[i];
            Int64[] b = new Int64[300];
            b[1] = i;
            total += b[1];
        }
        Console.WriteLine(total);
    }
}
