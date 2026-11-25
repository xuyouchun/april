// Benchmark: delegates and lambdas that capture a variable.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 offset = 3;
        Func<Int32, Int32> add = x => x + offset;
        Func<Int32, Int32> twice = x => add(add(x));

        Int64 total = 0;
        for (Int32 i = 0; i < 10000000; i++)
            total += twice(i);

        Console.WriteLine(total);
    }
}
