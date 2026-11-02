// EXPECT_WARNING 4006 4006 4006 4008

/* EXPECT

*/

#nullable enable

import System;

using System;
using System.Reflection;

class Program
{
    static void Take(String s) { }

    // Never called.
    static void Library(String? maybe, Exception e, Type t)
    {
        // The params that can be null.
        Console.WriteLine(maybe);
        Console.Write(maybe);
        String s = "a" + maybe;
        String c = String.Concat(maybe, maybe);

        // The values that can be null.
        Console.WriteLine(e.InnerException.Message);        // 4006
        Console.WriteLine(t.GetField("x").Name);            // 4006
        Console.WriteLine(t.Assembly.Name);                 // 4006

        // A param that cannot be null.
        Take(e.Message);                                    // 4008: the message can be null
    }

    [EntryPoint]
    public static void Main()
    {
    }
}
