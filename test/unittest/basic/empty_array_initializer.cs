// An array that is made with an empty initializer is empty: new Int32[] { }.

/* EXPECT

a 0 1
b 0 2
c 0 3
d 0
e 0
f 2 3 6
g 1 1
h True

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32[] a = new Int32[] { };
        Int32[,] b = new Int32[,] { };
        Int32[,,] c = new Int32[,,] { };
        Int32[] d = new Int32[0] { };
        String[] e = new String[] { };

        // The others are as they were.
        Int32[,] f = new Int32[,] { { 1, 2, 3 }, { 4, 5, 6 } };
        Int32[] g = new Int32[] { 7, };

        Console.WriteLine("a " + a.Length + " " + a.Rank);
        Console.WriteLine("b " + b.Length + " " + b.Rank);
        Console.WriteLine("c " + c.Length + " " + c.Rank);
        Console.WriteLine("d " + d.Length);
        Console.WriteLine("e " + e.Length);
        Console.WriteLine("f " + f.GetLength(0) + " " + f.GetLength(1) + " " + f.Length);
        Console.WriteLine("g " + g.Length + " " + g.Rank);

        // An object that is made with a constructor and braces is still an object.
        Box box = new Box() { };
        Console.WriteLine("h " + (box != null));
    }
}

class Box
{
}
