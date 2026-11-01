// Tuple<...> is a struct that implements ITuple<...> with the same types; its constructor sets the
// items, that are Item1, Item2 ...

/* EXPECT

10
x
True
3
2
0
5
10
x
True

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Tuple<int, String, bool> t = new Tuple<int, String, bool>(10, "x", true);
        Console.WriteLine(t.Item1);
        Console.WriteLine(t.Item2);
        Console.WriteLine(t.Item3);
        Console.WriteLine(t.Count);

        Tuple<int, String> d = default(Tuple<int, String>);
        ITuple<int, String> b = d;
        Console.WriteLine(b.Count);
        Console.WriteLine(d.Item1);

        Tuple<int> one = new Tuple<int>(5);
        Console.WriteLine(one.Item1);

        // The items are read through the interface of the boxed tuple.
        ITuple<int, String, bool> it = t;
        Console.WriteLine(it.Item1);
        Console.WriteLine(it.Item2);
        Console.WriteLine(it.Item3);
    }
}
