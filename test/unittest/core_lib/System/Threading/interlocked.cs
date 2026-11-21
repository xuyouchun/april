// Interlocked and Volatile: the counts that many threads make, Exchange and CompareExchange of numbers and of references.

/* EXPECT

count 8000 16000 -8000
exchange 5 7 9
compare 7 7 8
reference True True True
volatile 12 30

*/

import System;

using System;
using System.Threading;

class Node
{
    public Int32 Id;
}

class Program
{
    static Int32 __count;
    static Int64 __big;
    static Int32 __down;
    static Node? __node;

    static Void Work()
    {
        for (Int32 i = 0; i < 2000; i++)
        {
            Interlocked.Increment(ref __count);
            Interlocked.Add(ref __big, 2);
            Interlocked.Decrement(ref __down);
            Thread.Yield();
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Thread[] threads = new Thread[4];
        for (Int32 i = 0; i < 4; i++)
        {
            threads[i] = new Thread(Work);
            threads[i].Start();
        }

        for (Int32 i = 0; i < 4; i++)
            threads[i].Join();

        Console.WriteLine("count " + __count + " " + __big + " " + __down);

        Int32 value = 5;
        Int32 first = Interlocked.Exchange(ref value, 7);
        Int64 wide = 9;
        Int64 second = Interlocked.Exchange(ref wide, 11);
        Console.WriteLine("exchange " + first + " " + value + " " + second);

        Int32 a = Interlocked.CompareExchange(ref value, 100, 6);
        Int32 b = Interlocked.CompareExchange(ref value, 8, 7);
        Console.WriteLine("compare " + a + " " + b + " " + value);

        Node n1 = new Node(), n2 = new Node();
        Node? old1 = Interlocked.CompareExchange<Node?>(ref __node, n1, null);
        Node? old2 = Interlocked.CompareExchange<Node?>(ref __node, n2, null);
        Node? old3 = Interlocked.Exchange<Node?>(ref __node, n2);
        Console.WriteLine("reference " + (old1 == null) + " " + Object.ReferenceEquals(old2, n1) + " " + Object.ReferenceEquals(old3, n1));

        Int32 v = 12;
        Int64 w = 30;
        Console.WriteLine("volatile " + Volatile.Read(ref v) + " " + Volatile.Read(ref w));
    }
}
