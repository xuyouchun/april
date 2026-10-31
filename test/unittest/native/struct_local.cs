// Structs: locals, fields and copies.

/* EXPECT

5
7
12
1
100
2
True

5000000000
5
200

*/

import System;

using System;

struct Point
{
    public int X;
    public int Y;
}

struct Mixed
{
    public bool Flag;
    public long Big;
    public double Real;
    public byte Small;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Fields of a local.
        {
            Point p;
            p.X = 5;
            p.Y = 7;
            Console.WriteLine(p.X);
            Console.WriteLine(p.Y);
            Console.WriteLine(p.X + p.Y);
        }

        // Copies are independent.
        {
            Point a;
            a.X = 1;
            a.Y = 2;

            Point b;
            b = a;
            b.X = 100;

            Console.WriteLine(a.X);
            Console.WriteLine(b.X);
            Console.WriteLine(b.Y);
        }

        // Fields of different types.
        {
            Mixed m;
            m.Flag = true;
            m.Big = 5000000000;
            m.Real = 2.5;
            m.Small = (byte)200;

            Mixed n;
            n = m;

            Console.WriteLine(n.Flag);
            Console.WriteLine(n.Big);
            Console.WriteLine(n.Real * 2.0);
            Console.WriteLine(n.Small);
        }
    }
}
