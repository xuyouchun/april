// in params are read by the method (they are passed as values, a call can write in x), ref locals are another name of a variable.

/* EXPECT

in 5 8
in 5 8
in struct 3
in named 6
ref local 9 9
ref write 10 10
ref param 4 4
ref var 7
ref field 5
ref chain 12 12 12

*/

import System;

using System;

struct Point
{
    public Int32 X;
    public Int32 Y;
}

class Holder
{
    public Int32 Field = 1;

    public Int32 UseField()
    {
        ref Int32 f = ref Field;
        f = 5;
        return Field;
    }
}

class Program
{
    static Int32 Add(in Int32 a, in Int32 b) { return a + b; }

    static Int32 Sum(in Point p) { return p.X + p.Y; }

    static Int32 Twice(Int32 x, in Int32 factor = 2) { return x * factor; }

    static Int32 Bump(ref Int32 value)
    {
        ref Int32 alias = ref value;
        alias += 1;
        return value;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 a = 5, b = 3;
        Console.WriteLine("in " + a + " " + Add(a, b));
        Console.WriteLine("in " + a + " " + Add(in a, in b));

        Point p = new Point();
        p.X = 1;
        p.Y = 2;
        Console.WriteLine("in struct " + Sum(in p));

        Console.WriteLine("in named " + Twice(x: 3, factor: 2));

        Int32 x = 9;
        ref Int32 r = ref x;
        Console.WriteLine("ref local " + r + " " + x);

        r = 10;
        Console.WriteLine("ref write " + r + " " + x);

        Int32 z = 3;
        Console.WriteLine("ref param " + Bump(ref z) + " " + z);

        Int32 n = 7;
        ref var v = ref n;
        Console.WriteLine("ref var " + v);

        Console.WriteLine("ref field " + new Holder().UseField());

        Int32 c = 12;
        ref Int32 first = ref c;
        ref Int32 second = ref first;
        Console.WriteLine("ref chain " + c + " " + first + " " + second);
    }
}
