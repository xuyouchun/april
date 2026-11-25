// Benchmark: a struct that is copied and calculated.

import System;

using System;

struct Vec
{
    public Int32 X;
    public Int32 Y;

    public Vec(Int32 x, Int32 y) { X = x; Y = y; }

    public Vec Add(Vec other) { return new Vec(X + other.X, Y + other.Y); }

    public Int32 Dot(Vec other) { return X * other.X + Y * other.Y; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Vec acc = new Vec(0, 0);
        Vec step = new Vec(1, 2);
        Int64 dots = 0;

        for (Int32 i = 0; i < 10000000; i++)
        {
            acc = acc.Add(step);
            dots += acc.Dot(step) % 11;
        }

        Console.WriteLine(acc.X + " " + acc.Y + " " + dots);
    }
}
