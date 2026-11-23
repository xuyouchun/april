// A method or a lambda is converted to a nullable delegate (Delegate<..>? d = F;), as it is to a delegate. A nullable delegate is
// read into a variable to be called.

/* EXPECT

4
True
10
True
False
10
True
8
True

*/

import System;

using System;

class Program
{
    static Int32 F(Int32 x) { return x + 1; }

    static Boolean Use(Delegate<Int32, Int32>? d, Int32 v)
    {
        if (d == null)
            return false;

        Delegate<Int32, Int32> g = d.Value;
        Console.WriteLine(g(v));
        return true;
    }

    [EntryPoint]
    public static void Main()
    {
        Delegate<Int32, Int32>? d = F;
        Delegate<Int32, Int32>? f = (x) => x * 2;
        Delegate<Int32, Int32>? e = null;

        Console.WriteLine(Use(d, 3));
        Console.WriteLine(Use(f, 5));
        Console.WriteLine(Use(e, 1));
        Console.WriteLine(Use(F, 9));
        Console.WriteLine(Use(x => x - 1, 9));
    }
}
