/* EXPECT_ERROR is not a delegate */

import System;

using System;
using Handler = System.Delegate<void, Int32>;
using Multi = System.MulticastDelegate<void, Int32>;

class Button
{
    public event Int32 Clicked;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("x");
    }
}
