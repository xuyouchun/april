/* EXPECT_ERROR cannot be compiled: params of structs are not supported */

import System;

using System;

struct Point
{
    public Int32 X;

    public Point(Int32 x)
    {
        X = x;
    }
}

class Program
{
    static Int32 Count(params Point[] points)
    {
        return points.Length;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Count(new Point(1), new Point(2)));
    }
}
