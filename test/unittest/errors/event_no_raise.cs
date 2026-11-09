/* EXPECT_ERROR it has no raise accessor */

import System;

using System;
using Handler = System.Delegate<void, Int32>;
using Multi = System.MulticastDelegate<void, Int32>;

class Button
{
    private Multi __list;

    public event Handler Clicked
    {
        add { __list = __list + value; }
        remove { __list = __list - value; }
    }

    public void Click() { Clicked(1); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("x");
    }
}
