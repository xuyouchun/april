// Tuple<...> is a struct that implements ITuple<...> (and ITuple) with the same types; its
// constructor sets the items, that are Item1, Item2 ... or Get(index). ITuple<...>.Create makes
// a tuple in the heap as a reference type, without boxing.

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
10
x
True
3
7
hi
7
False
3
hi
out of range

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

        // Get returns the item as an object, a value is boxed.
        Console.WriteLine((int)t.Get(0));
        Console.WriteLine((String)t.Get(1));
        Console.WriteLine((bool)t.Get(2));

        // Create makes the tuple in the heap.
        ITuple<int, String, bool> c = ITuple.Create(7, "hi", false);
        Console.WriteLine(c.Count);
        Console.WriteLine(c.Item1);
        Console.WriteLine(c.Item2);
        Console.WriteLine((int)c.Get(0));
        Console.WriteLine((bool)c.Get(2));

        // ITuple is the base of all tuples.
        ITuple any = c;
        Console.WriteLine(any.Count);
        Console.WriteLine((String)any.Get(1));

        try
        {
            any.Get(3);
        }
        catch (OutOfRangeException)
        {
            Console.WriteLine("out of range");
        }
    }
}
