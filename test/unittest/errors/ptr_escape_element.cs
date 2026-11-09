/* EXPECT_ERROR (memory of the method) */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 5;
        Ptr<Int32>[] table = new Ptr<Int32>[2];
        table[0] = Ptr.AddressOf(ref n);
        Console.WriteLine(table[0].Value);
    }
}
