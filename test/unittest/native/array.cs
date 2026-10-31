// Arrays: elements of several types, dimensions and lengths.

/* EXPECT

10
285
81
26
4000000001
5
0
False

True

23
10
12
2
3
4
16
True

*/

import System;

using System;

class Item
{
    public Item(int value)
    {
        Value = value;
    }

    public int Value;
}

class Program
{
    static int Sum(int[] values)
    {
        int sum = 0;
        for (int k = 0; k < values.Length; k++)
            sum = sum + values[k];

        return sum;
    }

    [EntryPoint]
    public static void Main()
    {
        // Integers, with a loop.
        {
            int[] a = new int[10];
            for (int k = 0; k < a.Length; k++)
                a[k] = k * k;

            Console.WriteLine(a.Length);
            Console.WriteLine(Sum(a));
            Console.WriteLine(a[9]);
        }

        // Initializer.
        {
            int[] a = new int[4] { 5, 6, 7, 8 };
            Console.WriteLine(Sum(a));
        }

        // Other element types.
        {
            long[] l = new long[3];
            l[0] = 4000000000;
            l[2] = l[0] + 1;
            Console.WriteLine(l[2]);

            double[] d = new double[2];
            d[1] = 2.5;
            Console.WriteLine(d[1] * 2.0);
            Console.WriteLine(d[0]);

            bool[] b = new bool[2];
            b[1] = true;
            Console.WriteLine(b[0]);
            Console.WriteLine(b[1]);
        }

        // Two dimensions.
        {
            int[,] m = new int[3, 4];
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 4; j++)
                    m[i, j] = i * 10 + j;
            }

            Console.WriteLine(m[2, 3]);
            Console.WriteLine(m[1, 0]);
            Console.WriteLine(m.Length);
            Console.WriteLine(m.Rank);
            Console.WriteLine(m.GetLength(0));
            Console.WriteLine(m.GetLength(1));
        }

        // References.
        {
            Item[] items = new Item[3];
            items[0] = new Item(7);
            items[2] = new Item(9);

            Console.WriteLine(items[0].Value + items[2].Value);
            Console.WriteLine(items[1] == null);
        }
    }
}
