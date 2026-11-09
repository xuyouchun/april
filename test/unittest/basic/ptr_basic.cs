// Ptr and Ptr<T>: an address that is a value of 8 bytes (locals, fields, parameters, arrays, structs, generics).

/* EXPECT

4096
4096 True False True
4096
4104
4111
True 4096 4104
4104
True True True
0x0000000000001000
True

*/

import System;

using System;

struct Point
{
    public Int32 X;
    public Int32 Y;
}

struct Holder
{
    public Ptr<Int32> P;
    public Int32 N;
}

class Program
{
    static Ptr<Int32> Next(Ptr<Int32> p, Int64 step) => Ptr<Int32>.FromInt64(p.ToInt64() + step);

    static T Same<T>(T value) where T : unmanaged => value;

    [EntryPoint]
    public static void Main()
    {
        Ptr<Int32> p = Ptr<Int32>.FromInt64(4096);
        Console.WriteLine(p.ToInt64());

        Ptr q = p.ToPtr();
        Console.WriteLine(q.ToInt64() + " " + (q == Ptr.FromInt64(4096)) + " " + q.IsNull + " " + Ptr.Null.IsNull);

        Ptr<Double> d = q.As<Double>();
        Console.WriteLine(d.ToInt64());

        Ptr<Int32> n = Next(p, 8);
        Console.WriteLine(n.ToInt64());

        Holder h = new Holder();
        h.P = n;
        h.N = 7;
        Console.WriteLine(h.P.ToInt64() + h.N);

        Ptr<Int32>[] array = new Ptr<Int32>[3];
        array[1] = p;
        array[2] = n;
        Console.WriteLine(array[0].IsNull + " " + array[1].ToInt64() + " " + array[2].ToInt64());

        Ptr<Int32> same = Same(n);
        Console.WriteLine(same.ToInt64());

        Console.WriteLine((p == Ptr<Int32>.FromInt64(4096)) + " " + (p != n) + " " + p.Equals(Ptr<Int32>.FromInt64(4096)));

        Console.WriteLine(p.ToString());

        Ptr<Point> points = Ptr<Point>.Null;
        Console.WriteLine(points.IsNull);
    }
}
