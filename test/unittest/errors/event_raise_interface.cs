/* EXPECT_ERROR only added and removed */

import System;

using System;
typedef Handler = System.Delegate<void, Int32>;
typedef Multi = System.MulticastDelegate<void, Int32>;

interface IButton
{
    event Handler Clicked
    {
        add;
        remove;
        raise(Int32 x);
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
