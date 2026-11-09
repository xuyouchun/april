// A lambda in a lambda: the params of both have no types, and the return type of the inner one
// gives the generic param of the method that it is the argument of (ys.Select(y => y + x)).

/* EXPECT

run 2
outer 5,4,3
captured 11,21,12,22
three 1
block 4
done

*/

import System;

using System;
using System.Collections;
using System.Linq;

class Program
{
    static Int32 Run(Delegate<Int32, Int32> f) => f(1);

    static String Join(IEnumerable<Int32> items)
    {
        String text = "";
        foreach (Int32 item in items)
            text = text + (text == ""? "" : ",") + item;

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32[] xs = new Int32[] { 5, 1, 4, 2, 3 };
        Int32[] ys = new Int32[] { 10, 20 };

        Console.WriteLine("run " + Run(x => ys.Select(y => y).Count()));
        Console.WriteLine("outer " + Join(xs.Where(x => x > ys.Select(y => y).Count())));
        Console.WriteLine("captured " + Join(xs.Where(x => x < 3).SelectMany(x => ys.Select(y => y + x))));

        // Three levels.
        Console.WriteLine("three " + xs.Where(x => ys.Where(y => new Int32[] { 1, 2 }.Select(z => z * y).Count() > x).Count() > 0).Count());

        // A block as the body.
        Console.WriteLine("block " + Run(x =>
        {
            Int32 n = ys.Select(y => y + x).Count();
            return n + ys.Where(y => y > x).Select(y => y).Count();
        }));

        Console.WriteLine("done");
    }
}
