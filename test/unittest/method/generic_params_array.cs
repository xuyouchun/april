// A generic method that takes a params array of its generic parameter: the items are given one by one
// (the type is given or told by them), or as an array.

/* EXPECT

a 3
b 3
c 2
d 4
e 3
f k2
g 0
h 1.5

*/

import System;

using System;

public static class Tools
{
    public static Int32 Count<T>(params T[] items) { return items.Length; }

    public static String Kind<T>(String tag, params T[] items) { return tag + items.Length; }

    public static T First<T>(params T[] items) { return items[0]; }
}

public class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("a " + Tools.Count<String>(new String[] { "s", "t", "u" }));
        Console.WriteLine("b " + Tools.Count<String>("s", "t", "u"));
        Console.WriteLine("c " + Tools.Count("s", "t"));
        Console.WriteLine("d " + Tools.Count(new Int32[] { 1, 2, 3, 4 }));
        Console.WriteLine("e " + Tools.Count(1, 2, 3));
        Console.WriteLine("f " + Tools.Kind("k", 1.5, 2.5));
        Console.WriteLine("g " + Tools.Count<Int32>());
        Console.WriteLine("h " + Tools.First(1.5, 2.5));
    }
}
