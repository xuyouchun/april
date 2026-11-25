// Benchmark: recursive calls.

import System;

using System;

class Program
{
    static Int32 Fib(Int32 n)
    {
        if (n < 2)
            return n;

        return Fib(n - 1) + Fib(n - 2);
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Fib(35));
    }
}
