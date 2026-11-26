// Benchmark of the garbage collector: short lived small objects, almost everything dies young.
// Run with APRIL_GC_STATS=1 (pauses) or APRIL_GC_TRACE=1 (a line per collection); APRIL_GC=0 turns the collector off.

import System;
using System;

class Node { public Node Next; public Int64 V; }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int64 total = 0;
        for (Int32 i = 0; i < 6000000; i++)
        {
            Node n = new Node();
            n.V = i;
            total += n.V;
        }
        Console.WriteLine(total);
    }
}
