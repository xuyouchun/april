// Benchmark: integer arithmetic in nested loops.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int64 total = 0;

        for (Int32 i = 0; i < 40000; i++)
        {
            for (Int32 j = 0; j < 1000; j++)
            {
                total += (i * j + 7) % 13;
                if ((i ^ j) > 1500)
                    total -= 1;
            }
        }

        Console.WriteLine(total);
    }
}
// d
