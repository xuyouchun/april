// new[] { ... }: the type of the elements is the one that all the elements are converted to; new[,] { { ... } } is a matrix.

/* EXPECT

3 3
1 2.5
3 z
2
3 4
3 2
5
8
ab
6
3 c
13

*/

import System;

using System;

class P { public Int32 X; }

class Program
{
    static Int32[] Fixed = new[] { 7, 8, 9 };

    static Int32 Sum(Int32[] values)
    {
        Int32 total = 0;
        for (Int32 i = 0; i < values.Length; i++)
            total += values[i];

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        var a = new[] { 1, 2, 3 };
        Console.WriteLine(a.Length + " " + a[2]);

        var b = new[] { 1, 2.5 };                   // Double
        Console.WriteLine(b[0] + " " + b[1]);

        var c = new[] { "x", null, "z" };           // String
        Console.WriteLine(c.Length + " " + c[2]);

        var d = new[] { 1, 2L };                    // Int64
        Console.WriteLine(d[1]);

        var m = new[,] { { 1, 2 }, { 3, 4 } };
        Console.WriteLine(m[1, 0] + " " + m.Length);

        var j = new[] { new[] { 1 }, new[] { 2, 3 } };
        Console.WriteLine(j[1][1] + " " + j.Length);

        var p = new[] { new P(), new P() };
        p[1].X = 5;
        Console.WriteLine(p[1].X);

        Int32 k = 7;
        var e = new[] { k, k + 1 };
        Console.WriteLine(e[1]);

        foreach (var s in new[] { "a", "b" })
            Console.Write(s);
        Console.WriteLine();

        Console.WriteLine(Sum(new[] { 1, 2, 3 }));

        Char[] letters = new[] { 'a', 'b', 'c' };
        Console.WriteLine(letters.Length + " " + letters[2]);

        Console.WriteLine(Fixed[0] + Fixed[1] - Fixed[2] + 4 + 0 + Sum(new[] { 1, 2 }) - 3 + 3);
    }
}
