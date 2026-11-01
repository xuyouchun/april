// & | ^ of booleans evaluate both operands.

/* EXPECT

False

True

True

False

False

True

False

x
y
True

z
True

*/

import System;

using System;
class Program
{
    static bool Side(String s)
    {
        Console.WriteLine(s);
        return true;
    }

    [EntryPoint]
    public static void Main()
    {
        bool a = true;
        bool b = false;
        Console.WriteLine(a & b);
        Console.WriteLine(a | b);
        Console.WriteLine(a ^ b);
        Console.WriteLine(a ^ a);
        bool c = a;
        c &= b;
        Console.WriteLine(c);
        c |= a;
        Console.WriteLine(c);
        c ^= a;
        Console.WriteLine(c);
        // both operands are evaluated
        bool r = Side("x") & Side("y");
        Console.WriteLine(r);
        bool s = false | Side("z");
        Console.WriteLine(s);
    }
}
