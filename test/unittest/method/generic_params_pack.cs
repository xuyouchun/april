// A generic method takes any count of generic arguments by a params generic parameter:
// F<T ...>(T ... items). The types are inferred from the arguments, or are given.

/* EXPECT

3
a
5
x
0
2
2
True
7

*/

import System;

using System;

class Maker
{
    public static String First<T ...>(String key, T ... items) { return key; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Inferred.
        ITuple<int, String, bool> a = ITuple.Create(5, "x", true);
        Console.WriteLine(a.Count);
        Console.WriteLine(Maker.First("a", 1, 2.5, "s"));
        Console.WriteLine(a.Item1);
        Console.WriteLine(a.Item2);

        // No items.
        ITuple none = ITuple.Create();
        Console.WriteLine(none.Count);

        // Given.
        ITuple<int, String> two = ITuple.Create<int, String>(1, "z");
        Console.WriteLine(two.Count);
        Console.WriteLine(Maker.First<int, int>("k", 1, 2) == "k"? 2 : 0);
        Console.WriteLine(a.Item3);

        // The type is made from the argument.
        ITuple<int> one = ITuple.Create(7);
        Console.WriteLine(one.Item1);
    }
}
