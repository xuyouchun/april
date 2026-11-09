/* EXPECT_ERROR (unmanaged) */

import System;

using System;

class Program
{
    static T Same<T>(T value) where T : unmanaged => value;

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Same("text"));
    }
}
