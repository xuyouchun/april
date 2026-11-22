/* EXPECT_ERROR an interpolation of a string has no expression */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine($"a { } b");
    }
}
