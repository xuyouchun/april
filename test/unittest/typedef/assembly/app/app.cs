// The public aliases of a library.

import System;
import lib;

using System;
using lib.Geo;

class Program
{
    static Int32 Add(Int32 a, Int32 b) { return a + b; }

    [EntryPoint]
    public static void Main()
    {
        SBox<Int32> a = new SBox<Int32>();
        a.V = 5;
        Console.WriteLine(a.V);

        Cell c = new Cell();
        c.V = 6;
        Console.WriteLine(c.V);

        Entry<Int32> e = new Entry<Int32>();
        e.First = 7;
        e.Second = "x";
        Console.WriteLine(e.First + e.Second);

        Holder.Items items = new Holder.Items();
        items.Add(1);
        items.Add(2);
        Console.WriteLine(Holder.Total(items));

        Func<Int32, Int32, Int32> f = Add;
        Console.WriteLine(f(3, 4));

        Console.WriteLine(Holder.Secrets() + Holder.Inner());
    }
}
