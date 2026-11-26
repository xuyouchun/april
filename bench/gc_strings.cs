// Benchmark of the garbage collector: strings that are made and dropped (concatenation).
// Run with APRIL_GC_STATS=1 (pauses) or APRIL_GC_TRACE=1 (a line per collection); APRIL_GC=0 turns the collector off.

import System;
using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int64 total = 0;
        for (Int32 i = 0; i < 600000; i++)
        {
            String s = "item" + i + "-" + (i * 2);
            total += s.Length;
        }
        Console.WriteLine(total);
    }
}
