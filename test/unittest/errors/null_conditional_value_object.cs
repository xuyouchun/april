/* EXPECT_ERROR cannot be null */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int i = 1;
        Console.WriteLine(i?.ToString());
    }
}
