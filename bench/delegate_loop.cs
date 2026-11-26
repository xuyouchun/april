// Benchmark: a delegate that is called in a loop of a method (the lambda is the same for all the calls).

import System;

using System;

class Program
{
    static Int64 Run(Func<Int32, Int32> f, Int32 n)
    {
        Int64 total = 0;
        for (Int32 i = 0; i < n; i++)
            total += f(i & 255);

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 offset = 3;
        Func<Int32, Int32> plus = x => x * 2 + offset;
        Console.WriteLine(Run(plus, 20000000));
    }
}
