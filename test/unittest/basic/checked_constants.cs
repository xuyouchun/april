// checked: the constants that fit are compiled, the arithmetic of variables is checked when it runs.

/* EXPECT

2147483647
2147395600
-2147483648
2147483647
255
1999999999
-2147483648
-2147483648
-2147483648
overflow

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Constants that fit.
        Console.WriteLine(checked(2147483646 + 1));
        Console.WriteLine(checked(46340 * 46340));
        Console.WriteLine(checked(-2147483647 - 1));
        Console.WriteLine(checked(-(-2147483647)));
        Console.WriteLine(checked((byte)255));
        Console.WriteLine(checked((int)2000000000L - 1));

        // Unchecked inside: wraps, and the value is used.
        Console.WriteLine(checked(unchecked(2147483647 + 1) + 0));
        Console.WriteLine(unchecked(2147483647 + 1));

        // Not checked: wraps.
        Console.WriteLine(2147483647 + 1);

        // A constant that does not fit with a variable: it is checked when it runs.
        int one = 1;
        try { Console.WriteLine(checked(2147483647 + one)); }
        catch (OverflowException) { Console.WriteLine("overflow"); }
    }
}
