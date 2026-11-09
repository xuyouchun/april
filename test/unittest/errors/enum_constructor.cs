/* EXPECT_ERROR cannot have a constructor */

import System;

using System;

enum Size
{
    Small, Large;

    public Size(Int32 v) { }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("x");
    }
}
