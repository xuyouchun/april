/* EXPECT_ERROR is of the type "System.Int32", it is not a delegate and cannot be called */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 x = 3;
        Console.WriteLine(x(5));
    }
}
