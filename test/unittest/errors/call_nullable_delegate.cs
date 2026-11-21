/* EXPECT_ERROR it is not a delegate and cannot be called */

import System;

using System;

class Program
{
    static Int32 Twice(Int32 x) { return x * 2; }

    [EntryPoint]
    public static void Main()
    {
        Delegate<Int32, Int32> f = new Delegate<Int32, Int32>(Twice);
        Delegate<Int32, Int32>? d = f;
        Console.WriteLine(d(5));
    }
}
