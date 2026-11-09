// The items of a tuple can be custom structs: they are read, passed on, boxed (Get) and used through a generic param.

/* EXPECT

3 6 5 10
10
1 5
5 7 6
b 4
12 13
done

*/

import System;

using System;

interface INext<TSelf> where TSelf : INext<TSelf>
{
    static abstract Tuple<TSelf, TSelf> Pair(TSelf v);
}

struct Pt : INext<Pt>
{
    public Int32 V;
    public Int32 W;

    public static Pt Make(Int32 v)
    {
        Pt p = new Pt();
        p.V = v;
        p.W = v * 2;
        return p;
    }

    public static Tuple<Pt, Pt> Pair(Pt v)
    {
        Pt w = new Pt();
        w.V = v.V + 1;
        return new Tuple<Pt, Pt>(v, w);
    }
}

class Util
{
    public static T Second<T>(T a) where T : INext<T>
    {
        Tuple<T, T> t = T.Pair(a);
        T r = t.Item2;
        return r;
    }

    public static T Last<T>(T a, T b)
    {
        Tuple<T, T> t = new Tuple<T, T>(a, b);
        return t.Item2;
    }

    static Int32 Sum(Pt a, Pt b) { return a.V + b.V; }

    public static Int32 FromTuple(Tuple<Pt, Pt> t) { return Sum(t.Item1, t.Item2); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Pt a = Pt.Make(3);
        Pt b = Pt.Make(5);
        Tuple<Pt, Pt> t = new Tuple<Pt, Pt>(a, b);
        Pt x = t.Item1;
        Pt y = t.Item2;
        Console.WriteLine(x.V + " " + x.W + " " + y.V + " " + y.W);
        Console.WriteLine(t.Item2.W);

        Tuple<Int32, Pt> u = new Tuple<Int32, Pt>(1, b);
        Console.WriteLine(u.Item1 + " " + u.Item2.V);

        Object boxed = t.Get(1);
        Pt back = (Pt)boxed;
        Pt s = Util.Second(Pt.Make(4));
        Console.WriteLine(back.V + " " + Util.Last(Pt.Make(7), Pt.Make(7)).V + " " + (s.V + 1));
        Console.WriteLine(Util.Last("a", "b") + " " + Util.Last(3, 4));
        Console.WriteLine(Util.FromTuple(new Tuple<Pt, Pt>(Pt.Make(5), Pt.Make(7))) + " " + (Util.FromTuple(t) + 5));
        Console.WriteLine("done");
    }
}
