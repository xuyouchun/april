// Tuple<...> is a struct that implements ITuple<...> with the same types.

/* EXPECT

2
0
1

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Tuple<int, String> t = default(Tuple<int, String>);
        ITuple<int, String> b = t;

        Console.WriteLine(b.Count);
        Console.WriteLine(t.Item1);

        Tuple<int> one = default(Tuple<int>);
        Console.WriteLine(one.Count);
    }
}
