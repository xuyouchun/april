/* EXPECT_ERROR "lock" requires a reference: "System.Int32" */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 x = 3;
        lock (x) { Console.WriteLine("no"); }
    }
}
