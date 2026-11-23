/* EXPECT_ERROR is not a delegate */

import System;

using System;
typedef Handler = System.Delegate<void, Int32>;
typedef Multi = System.MulticastDelegate<void, Int32>;

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
