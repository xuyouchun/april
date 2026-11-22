/* EXPECT_ERROR a } of an interpolated string must be doubled */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 x = 1;
        Console.WriteLine($"{x} }");
    }
}
