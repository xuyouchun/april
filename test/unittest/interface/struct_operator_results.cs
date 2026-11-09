// The result of an operator of a struct (a method that returns a struct) is the object of a member, an argument, the
// operand of another operator, in a chain, in parentheses or not, and through a generic parameter.

/* EXPECT

2 21 13 13 21 -8 -13 26 52
15 15 -15 30
done

*/

import System;

using System;

interface IVec<TSelf> where TSelf : IVec<TSelf>
{
    static abstract TSelf operator +(TSelf a, TSelf b);

    static abstract TSelf operator -(TSelf a);
}

struct C : IVec<C>
{
    public Int32 V;

    public static C Make(Int32 v)
    {
        C c = new C();
        c.V = v;
        return c;
    }

    public static C operator +(C a, C b) { return Make(a.V + b.V); }

    public static C operator >>(C a, Int32 n) { return Make(a.V >> n); }

    public static C operator -(C a) { return Make(-a.V); }

    public C Twice() { return Make(V * 2); }
}

class Util
{
    public static T Sum3<T>(T a, T b, T c) where T : IVec<T> { return a + b + c; }

    public static T NegSum<T>(T a, T b) where T : IVec<T> { return -(a + b); }

    public static T Nested<T>(T a, T b) where T : IVec<T> { return (a + b) + (a + b); }
}

class Program
{
    static Int32 Take(C c) { return c.V; }

    [EntryPoint]
    public static void Main()
    {
        C a = C.Make(8), b = C.Make(5);

        Console.WriteLine((a >> 2).V + " " + (a + b + a).V + " " + (a + b).V + " " + Take(a + b) + " " + Take(a + b + a) + " "
            + (-a).V + " " + (-(a + b)).V + " " + (a + b).Twice().V + " " + (a + b).Twice().Twice().V);

        C g1 = Util.Sum3(C.Make(3), C.Make(5), C.Make(7));
        C g2 = Util.Nested(C.Make(3), C.Make(4));
        C g3 = Util.NegSum(C.Make(7), C.Make(8));
        Console.WriteLine(g1.V + " " + (g2.V + 1) + " " + g3.V + " " + (g2.V + 16));
        Console.WriteLine("done");
    }
}
