// Benchmark: the accessors of properties (a method that only reads or writes a field of this).

import System;

using System;

class Vec
{
    Int32 _x;
    Int32 _y;
    Double _w;

    public Int32 X { get { return _x; } set { _x = value; } }
    public Int32 Y { get { return _y; } set { _y = value; } }
    public Double W { get { return _w; } set { _w = value; } }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Vec v = new Vec();
        Int64 sum = 0;
        Double w = 0;

        for (Int32 i = 0; i < 20000000; i++)
        {
            v.X = i;
            v.Y = v.X + 1;
            v.W = v.Y * 0.5;
            sum += v.Y - v.X;
            w += v.W;
        }

        Console.WriteLine(sum);
        Console.WriteLine(w);
    }
}
