// A number cast to an enum whose underlying type is Int64 is extended with its sign.

/* EXPECT

-7
-7
-7
True
True
2

*/

import System;

using System;

enum Sign : Int64
{
    A = 1,
    N = -5
}

enum Small : Int32
{
    X = 2
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 k = -7;
        Sign s = (Sign)k;
        Console.WriteLine((Int64)s);
        Console.WriteLine((Int64)(Sign)(-7));

        Sign t = (Sign)(Int16)(-7);
        Console.WriteLine((Int64)t);

        Console.WriteLine(s == (Sign)(-7));
        Console.WriteLine(s < Sign.A);

        Console.WriteLine((Int64)(Sign)Small.X);
    }
}
