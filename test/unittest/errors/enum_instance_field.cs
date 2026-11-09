/* EXPECT_ERROR cannot have the field or event */

// An enum is its integer: it has no field of its own, only const ones.

import System;

using System;

enum Size
{
    Small, Large;

    public Int32 Extra;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("x");
    }
}
