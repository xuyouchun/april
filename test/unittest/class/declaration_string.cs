// The pattern of is with a string: o is string s.

/* EXPECT

5
not a string
False
2

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Object o = "hello";
        if (o is String s)
            Console.WriteLine(s.Length);

        Object n = 5;
        if (!(n is String t))
            Console.WriteLine("not a string");

        Object nothing = null;
        Console.WriteLine(nothing is String z);

        // The variable is assigned in the whole expression: a && b.
        Object m = "ab";
        if (m is String u && u.Length == 2)
            Console.WriteLine(u.Length);
    }
}
