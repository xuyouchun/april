// A struct (and a delegate) that is an item of the initializer of an array of references is boxed.

/* EXPECT

p 3 4
q 5 6
count 3
num 21
dg 42
copy 3 4

*/

import System;

using System;

struct P
{
    public Int32 X;
    public Int32 Y;
}

class Tool
{
    public Int32 Twice(Int32 x) { return x * 2; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        P p = new P();
        p.X = 3;
        p.Y = 4;

        P q = new P();
        q.X = 5;
        q.Y = 6;

        Tool tool = new Tool();
        Delegate<Int32, Int32> d = tool.Twice;

        Object?[] items = new Object?[] { p, 21, q, d };

        P a = (P)items[0];
        Console.WriteLine("p " + a.X + " " + a.Y);

        P b = (P)items[2];
        Console.WriteLine("q " + b.X + " " + b.Y);

        Console.WriteLine("count " + (items.Length - 1));
        Console.WriteLine("num " + items[1]);

        Delegate<Int32, Int32> f = (Delegate<Int32, Int32>)items[3];
        Console.WriteLine("dg " + f(21));

        // The box is a copy: the struct does not change when the variable does.
        p.X = 99;
        P c = (P)items[0];
        Console.WriteLine("copy " + c.X + " " + c.Y);
    }
}
