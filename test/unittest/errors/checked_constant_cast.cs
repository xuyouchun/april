/* EXPECT_ERROR The conversion overflows at compile time */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(checked((int)3000000000L));
    }
}
