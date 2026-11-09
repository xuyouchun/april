// A generic parameter that has an interface as a constraint has the static members of the interfaces that it
// inherits too (properties, methods, operators, at any depth); the shift operators (<< and >>) can be declared.

/* EXPECT

13 12 11 10 4 11
16 256
5 30
7

*/

import System;

using System;

interface I0<TSelf> where TSelf : I0<TSelf>
{
    static abstract TSelf P0 { get; }

    static abstract TSelf M0();

    static abstract TSelf operator +(TSelf left, TSelf right);
}

interface I1<TSelf> : I0<TSelf> where TSelf : I1<TSelf>
{
    static abstract TSelf P1 { get; }
}

interface I2<TSelf> : I1<TSelf> where TSelf : I2<TSelf>
{
    static abstract TSelf P2 { get; }
}

interface I3<TSelf> : I2<TSelf> where TSelf : I3<TSelf>
{
    static abstract TSelf P3 { get; }
}

interface IShift<TSelf> where TSelf : IShift<TSelf>
{
    static abstract TSelf operator <<(TSelf value, Int32 amount);

    static abstract TSelf operator >>(TSelf value, Int32 amount);
}

struct S : I3<S>
{
    public Int32 V;

    public static S Make(Int32 v)
    {
        S s = new S();
        s.V = v;
        return s;
    }

    public static S P0 { get { return Make(10); } }

    public static S P1 { get { return Make(11); } }

    public static S P2 { get { return Make(12); } }

    public static S P3 { get { return Make(13); } }

    public static S M0() { return Make(4); }

    public static S operator +(S left, S right) { return Make(left.V + right.V); }
}

struct Bits : IShift<Bits>
{
    public Int32 V;

    public static Bits Make(Int32 v)
    {
        Bits b = new Bits();
        b.V = v;
        return b;
    }

    public static Bits operator <<(Bits value, Int32 amount) { return Make(value.V << amount); }

    public static Bits operator >>(Bits value, Int32 amount) { return Make(value.V >> amount); }
}

class Util
{
    public static T G0<T>() where T : I3<T> { return T.P0; }

    public static T G1<T>() where T : I3<T> { return T.P1; }

    public static T G2<T>() where T : I3<T> { return T.P2; }

    public static T G3<T>() where T : I3<T> { return T.P3; }

    public static T Method<T>() where T : I3<T> { return T.M0(); }

    public static T Plus<T>(T a, T b) where T : I3<T> { return a + b; }

    public static T Left<T>(T a, Int32 n) where T : IShift<T> { return a << n; }

    public static T Right<T>(T a, Int32 n) where T : IShift<T> { return a >> n; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        S p3 = Util.G3<S>(), p2 = Util.G2<S>(), p1 = Util.G1<S>(), p0 = Util.G0<S>(), m = Util.Method<S>();
        S sum = Util.Plus(S.Make(5), S.Make(6));
        Console.WriteLine(p3.V + " " + p2.V + " " + p1.V + " " + p0.V + " " + m.V + " " + sum.V);

        Bits b = Bits.Make(64);
        Bits r = b >> 2, l = b << 2;
        Console.WriteLine(r.V + " " + l.V);

        Bits r2 = Util.Right(Bits.Make(20), 2), l2 = Util.Left(Bits.Make(15), 1);
        Console.WriteLine(r2.V + " " + l2.V);

        Int32 x = 28;
        Console.WriteLine(x >> 2);
    }
}
