/* EXPECT_ERROR has no "remove" accessor */

import System;

using System;
typedef Handler = System.Delegate<void, Int32>;
typedef Multi = System.MulticastDelegate<void, Int32>;

class Button
{
    private Multi __list;

    public event Handler Clicked
    {
        add { __list = __list + value; }
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("x");
    }
}
