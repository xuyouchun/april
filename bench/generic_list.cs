// Benchmark: a generic list, its items are added and read.

import System;

using System;
using System.Collections;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        List<Int32> list = new List<Int32>();

        for (Int32 i = 0; i < 1000000; i++)
            list.Add(i * 3);

        Int64 total = 0;
        for (Int32 round = 0; round < 10; round++)
        {
            for (Int32 i = 0; i < list.Count; i++)
                total += list[i];
        }

        Console.WriteLine(total);
    }
}
