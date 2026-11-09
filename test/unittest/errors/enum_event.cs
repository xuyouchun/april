/* EXPECT_ERROR cannot have the field or event */

// An enum has no place for the delegates of an event.

import System;

using System;

enum Size
{
    Small, Large;

    public static event Action Changed;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("x");
    }
}
