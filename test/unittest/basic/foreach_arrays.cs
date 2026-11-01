// foreach over arrays.

/* EXPECT

3
4
5
12
x
y
8
2
1
0
sum 21
found 2
6
a=1
a=2
b=3
0

*/

import System;

using System;

struct P
{
    public int X;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int[] a = new int[] { 3, 4, 5 };
        foreach (int v in a)
        {
            Console.WriteLine(v);
        }

        int total = 0;
        foreach (int v in a)
            total += v;
        Console.WriteLine(total);

        String[] s = new String[] { "x", "y" };
        foreach (String t in s)
            Console.WriteLine(t);

        // continue and break.
        int sum = 0;
        foreach (int v in a)
        {
            if (v == 4)
                continue;

            sum += v;
        }
        Console.WriteLine(sum);

        foreach (int v in new int[] { 2, 1, 0, 5, 6 })
        {
            if (v == 5)
                break;

            Console.WriteLine(v);
        }

        // Nested.
        int[][] j = new int[][] { new int[] { 1, 2 }, new int[] { 3, 4, 11 } };
        int all = 0;
        foreach (int[] row in j)
            foreach (int x in row)
                all += x;
        Console.WriteLine("sum " + all);

        // The variable of a statement is its own.
        int found = 0;
        foreach (int v in a)
        {
            if (v == 4)
                found = 2;
        }
        Console.WriteLine("found " + found);

        // Elements of a local that is changed in the body.
        int[] w = new int[] { 1, 2, 3 };
        foreach (int v in w)
            w[v - 1] = v * 2;
        Console.WriteLine(w[0] + w[1] + w[2] - 6);

        // var.
        foreach (var e in new int[] { 1, 2 })
            Console.WriteLine("a=" + e);

        foreach (int e in new int[] { 3 })
            Console.WriteLine("b=" + e);

        // Empty.
        int count = 0;
        foreach (int e in new int[0])
            count++;
        Console.WriteLine(count);
    }
}
