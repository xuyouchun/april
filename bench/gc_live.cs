// Benchmark of the garbage collector: a big live list of nodes plus garbage, each collection marks the whole live set.
// Run with APRIL_GC_STATS=1 (pauses) or APRIL_GC_TRACE=1 (a line per collection); APRIL_GC=0 turns the collector off.

import System;
using System;

class Node { public Node Next; public Int64 V; }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Node head = null;
        for (Int32 i = 0; i < 1500000; i++)
        {
            Node n = new Node();
            n.V = i; n.Next = head; head = n;
        }

        Int64 total = 0;
        for (Int32 i = 0; i < 4000000; i++)
        {
            Node t = new Node();
            t.V = i;
            total += t.V;
        }

        Int64 sum = 0;
        for (Node p = head; p != null; p = p.Next) sum += p.V;
        Console.WriteLine(total + " " + sum);
    }
}
