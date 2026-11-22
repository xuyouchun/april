// The value of an indexer (or of a property) that is taken as an Object is boxed: Object o = list[0], String.Format("{0}", list[0]).

/* EXPECT

box 7
format 7 8
array 7 7

*/

import System;

using System;
using System.Collections;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        List<Int32> list = new List<Int32>();
        list.Add(7);
        list.Add(8);

        Object boxed = list[0];
        Console.WriteLine("box " + boxed);
        Console.WriteLine(String.Format("format {0} {1}", list[0], list[1]));

        Object[] items = new Object[] { list[0], list.Count > 1? list[0] : list[1] };
        Console.WriteLine("array " + items[0] + " " + items[1]);
    }
}
