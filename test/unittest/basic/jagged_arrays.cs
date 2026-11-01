// Arrays of arrays, and arrays of references initialized with elements.

/* EXPECT

2
True
5
3
3
b
2
6
4
True

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int[][] j = new int [2][];
        Console.WriteLine(j.Length);
        Console.WriteLine(j[1] == null);

        j[0] = new int[3];
        j[0][1] = 5;
        Console.WriteLine(j[0][1]);
        Console.WriteLine(j[0].Length);

        // With initializers.
        int[][] k = new int[][] { new int[] { 1 }, new int[] { 2, 3, 4 } };
        Console.WriteLine(k[1][1]);

        String[] s = new String[] { "a", "b" };
        Console.WriteLine(s[1]);

        // Rows of different lengths.
        int[][] t = new int [3][];
        for (int i = 0; i < 3; i++)
        {
            t[i] = new int[i + 1];
            t[i][i] = i + 1;
        }

        Console.WriteLine(t[1].Length);
        Console.WriteLine(t[0][0] + t[1][1] + t[2][2]);

        // Arrays of three levels.
        int[][][] c = new int [2][][];
        c[0] = new int [2][];
        c[0][1] = new int[4];
        Console.WriteLine(c[0][1].Length);
        Console.WriteLine(c[1] == null);
    }
}
