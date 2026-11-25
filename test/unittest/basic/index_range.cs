// a[^1] is the last item, a[1..3] the items from the second to the third (the end is not one of them), ..2 and 1.. leave the start
// and the end. Arrays, strings and lists have them.

/* EXPECT

last 5
second last 4
slice 2 3
from start 1 2
to end 4 5
all 5
hat slice 3 4
string o
substring ell
string tail lo
list 30
list slice 2 20
empty 0
index 30
range text ^2..3

*/

import System;

using System;
using System.Collections;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32[] a = new Int32[] { 1, 2, 3, 4, 5 };

        Console.WriteLine("last " + a[^1]);
        Console.WriteLine("second last " + a[^2]);

        Int32[] mid = a[1..3];
        Console.WriteLine("slice " + mid[0] + " " + mid[1]);

        Int32[] head = a[..2];
        Console.WriteLine("from start " + head[0] + " " + head[1]);

        Int32[] tail = a[3..];
        Console.WriteLine("to end " + tail[0] + " " + tail[1]);

        Console.WriteLine("all " + a[..].Length);

        Int32[] hats = a[^3..^1];
        Console.WriteLine("hat slice " + hats[0] + " " + hats[1]);

        String s = "hello";
        Console.WriteLine("string " + s[^1]);
        Console.WriteLine("substring " + s[1..4]);
        Console.WriteLine("string tail " + s[3..]);

        List<Int32> list = new List<Int32> { 10, 20, 30 };
        Console.WriteLine("list " + list[^1]);

        List<Int32> part = list[1..];
        Console.WriteLine("list slice " + part.Count + " " + part[0]);

        Console.WriteLine("empty " + a[2..2].Length);

        Int32 k = 1;
        Console.WriteLine("index " + list[^k]);
        Console.WriteLine("range " + "text" + " " + new Range(Index.FromEnd(2), new Index(3, false)));
    }
}
