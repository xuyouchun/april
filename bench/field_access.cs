// Benchmark: reading and writing the fields of an object, and a property.

import System;

using System;

class Counter
{
    public Int32 A;
    public Int32 B;
    private Int32 __c;

    public Int32 C { get { return __c; } set { __c = value; } }

    public void Step(Int32 i)
    {
        A += i;
        B = B + A % 7;
        C = C + 1;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Counter counter = new Counter();

        for (Int32 i = 0; i < 10000000; i++)
            counter.Step(i);

        Console.WriteLine(counter.A + " " + counter.B + " " + counter.C);
    }
}
