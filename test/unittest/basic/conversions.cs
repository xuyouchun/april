// Conversions between integers and floating point numbers.

/* EXPECT

1.5
2
0.5
3
-3
5
1.5
7
3
1
201
2
4.5
3.5

*/

import System;

using System;

class Holder
{
    public int V = 3;
}

class Program
{
    static double Half(double x)
    {
        return x / 2;
    }

    static double FromInt()
    {
        int i = 7;
        return i;
    }

    [EntryPoint]
    public static void Main()
    {
        int i = 3;
        double d = i;
        Console.WriteLine(d / 2);

        double e = 2;
        Console.WriteLine(e);

        float f = 2;
        Console.WriteLine(f / 4);

        double x = 3.9;
        Console.WriteLine((int)x);

        x = -3.9;
        Console.WriteLine((int)x);

        long l = 5000000000;
        double dl = l;
        Console.WriteLine(dl / 1000000000.0);

        // Arguments and results.
        Console.WriteLine(Half(3));
        Console.WriteLine(FromInt());

        float g = 1.5f;
        double h = g;
        Console.WriteLine(h * 2);

        int k = 1;
        double t = true ? k : 2.5;
        Console.WriteLine(t);

        byte b = (byte)200;
        int bi = (int)b;
        Console.WriteLine(bi + 1);

        const double C = 2;
        Console.WriteLine(C);

        // A field.
        Holder holder = new Holder();
        double hv = holder.V;
        Console.WriteLine(hv * 1.5);

        // An integer argument of a floating point parameter, in an expression.
        Console.WriteLine(Half(i) + 2);
    }
}
