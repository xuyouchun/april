// The escapes of a character and of a string: \f is the form feed (12), \v the vertical tab (11).

/* EXPECT

7 8 9 10 11 12 13
12 11 102 118
3 12 11

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine((Int32)'\a' + " " + (Int32)'\b' + " " + (Int32)'\t' + " " + (Int32)'\n' + " " + (Int32)'\v' + " " + (Int32)'\f' + " " + (Int32)'\r');

        String s = "\f\v";
        Console.WriteLine((Int32)s[0] + " " + (Int32)s[1] + " " + (Int32)'f' + " " + (Int32)'v');

        String t = "a\fb\vc";
        Console.WriteLine(t.Length - 2 + " " + (Int32)t[1] + " " + (Int32)t[3]);
    }
}
