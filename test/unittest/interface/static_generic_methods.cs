// A static abstract (or virtual) method of an interface that has its own generic parameter (Create<TOther>), used
// through a generic parameter: T.Create(value), with the type of the value that is not the one of T, and a static
// virtual method of that kind that calls the abstract ones.

/* EXPECT

8 40 9

*/

import System;

using System;

interface INum<TSelf> where TSelf : INum<TSelf>
{
    static abstract Int64 ToInt64(TSelf v);
    static abstract TSelf Create<TOther>(TOther value) where TOther : INum<TOther>;
    static virtual TSelf Twice<TOther>(TOther value) where TOther : INum<TOther> { TSelf a = TSelf.Create<TOther>(value); return TSelf.Create<TSelf>(a); }
}

struct A : INum<A>
{
    public Int64 V;
    public static Int64 ToInt64(A v) { return v.V; }
    public static A Create<TOther>(TOther value) where TOther : INum<TOther> { A a = new A(); a.V = TOther.ToInt64(value) + 1; return a; }
}

struct B : INum<B>
{
    public Int64 V;
    public static Int64 ToInt64(B v) { return v.V; }
    public static B Create<TOther>(TOther value) where TOther : INum<TOther> { B b = new B(); b.V = TOther.ToInt64(value) * 10; return b; }
}

class Util
{
    public static T Convert<T, U>(U u) where T : INum<T> where U : INum<U> { return T.Create(u); }
    public static T Twice<T, U>(U u) where T : INum<T> where U : INum<U> { return T.Twice(u); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        A a = new A(); a.V = 4;
        B b = new B(); b.V = 7;
        A ab = Util.Convert<A, B>(b);
        B ba = Util.Convert<B, A>(a);
        A tw = Util.Twice<A, B>(b);
        Console.WriteLine(A.ToInt64(ab) + " " + B.ToInt64(ba) + " " + A.ToInt64(tw));
    }
}
