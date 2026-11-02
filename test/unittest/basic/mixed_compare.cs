// Integers of different signs are compared by value.

/* EXPECT

True
False
False
True
True
True
True

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int a = -1;
        int e = 5;
        uint b = (uint)1;
        uint f = (uint)5;
        short s = (short)-3;

        Console.WriteLine(a < b);
        Console.WriteLine(a == f);
        Console.WriteLine(a >= b);
        Console.WriteLine(e == f);
        Console.WriteLine(b > a);
        Console.WriteLine(s < f);
        Console.WriteLine(a != f);
    }
}
