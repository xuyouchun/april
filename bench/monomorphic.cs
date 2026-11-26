import System;
using System;

interface IScale
{
    Int32 Apply(Int32 v);
}

class Doubler : IScale
{
    public Int32 Apply(Int32 v) { return v * 2 + 1; }
}

abstract class Op
{
    public abstract Int32 Run(Int32 v);
}

class Adder : Op
{
    public Int32 K = 3;
    public override Int32 Run(Int32 v) { return v + K; }
}

class Program
{
    static Int64 Work(IScale s, Op op, Int32 n)
    {
        Int64 total = 0;
        for (Int32 i = 0; i < n; i++)
            total += s.Apply(i & 255) + op.Run(i & 127);

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Work(new Doubler(), new Adder(), 20000000));
    }
}
