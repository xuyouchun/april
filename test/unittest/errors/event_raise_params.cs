/* EXPECT_ERROR its params are not those of */

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
        remove { __list = __list - value; }
        raise(String text) { }
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
