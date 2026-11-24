// var (a, b) = tuple; declares the variables, of the items of the tuple. A name that is _ is not a variable: the item is
// not taken. foreach (var (a, b) in items) deconstructs each item.

/* EXPECT

1 two
named 3 x
skip 20
skip 7
call 5 hello
three 1 2 3
foreach 1 a, 2 b, 3 c,
foreach skip 1 2 3
foreach list 10 20 30
outer a 1
outer b 2

*/

import System;

using System;
using System.Collections;

class Program
{
    static (Int32, String) Pair() { return (5, "hello"); }

    [EntryPoint]
    public static void Main()
    {
        var (a, b) = (1, "two");
        Console.WriteLine(a + " " + b);

        (Int32 Number, String Name) named = (3, "x");
        var (n, s) = named;
        Console.WriteLine("named " + n + " " + s);

        var (_, second) = (10, 20);
        Console.WriteLine("skip " + second);

        var (first, _) = (7, 8);
        Console.WriteLine("skip " + first);

        var (id, text) = Pair();
        Console.WriteLine("call " + id + " " + text);

        var (x, y, z) = (1, 2, 3);
        Console.WriteLine("three " + x + " " + y + " " + z);

        // foreach: each item is deconstructed.
        var items = new (Int32, String)[] { (1, "a"), (2, "b"), (3, "c") };
        String line = "foreach";
        foreach (var (number, letter) in items)
            line += " " + number + " " + letter + ",";

        Console.WriteLine(line);

        var triples = new (Int32, Int32, Int32)[] { (1, 5, 9), (2, 6, 9), (3, 7, 9) };
        String skipped = "foreach skip";
        foreach (var (p, _, _) in triples)
            skipped += " " + p;

        Console.WriteLine(skipped);

        List<(Int32, Int32)> list = new List<(Int32, Int32)>();
        list.Add((10, 1));
        list.Add((20, 2));
        list.Add((30, 3));

        String sum = "foreach list";
        foreach (var (value, _) in list)
            sum += " " + value;

        Console.WriteLine(sum);

        // The variables are those of the loop: the names can be used again, and the body can have blocks.
        String[] names = new String[] { "a", "b" };
        Int32 count = 0;
        foreach (var (name, k) in new (String, Int32)[] { ("a", 1), ("b", 2) })
        {
            count++;
            Console.WriteLine("outer " + name + " " + k);
        }
    }
}
