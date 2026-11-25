// Benchmark: arrays, the sieve of Eratosthenes.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 20000000;
        Boolean[] composite = new Boolean[n + 1];
        Int32 count = 0;

        for (Int32 i = 2; i <= n; i++)
        {
            if (!composite[i])
            {
                count++;

                for (Int64 j = (Int64)i * i; j <= n; j += i)
                    composite[(Int32)j] = true;
            }
        }

        Console.WriteLine(count);
    }
}
