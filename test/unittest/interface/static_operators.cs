// The operators of an interface are static abstract members: a type that implements the
// interface defines them, they are used through a generic parameter that has the interface as a
// constraint (a + b, a == b, -a, a < b), the same as the members (T.MaxValue).

/* EXPECT

True
False
False
True
7
-3
True
False
4
9
75
True
5
10

*/

import System;

using System;

interface IEqualityOperators<TSelf, TOther, TResult>
    where TSelf : IEqualityOperators<TSelf, TOther, TResult>
{
    static abstract TResult operator ==(TSelf left, TOther right);

    static abstract TResult operator !=(TSelf left, TOther right);
}

interface IAdditionOperators<TSelf, TOther, TResult>
    where TSelf : IAdditionOperators<TSelf, TOther, TResult>
{
    static abstract TResult operator +(TSelf left, TOther right);
}

interface IUnaryNegationOperators<TSelf, TResult>
    where TSelf : IUnaryNegationOperators<TSelf, TResult>
{
    static abstract TResult operator -(TSelf value);
}

interface IComparisonOperators<TSelf, TOther, TResult>
    where TSelf : IComparisonOperators<TSelf, TOther, TResult>
{
    static abstract TResult operator <(TSelf left, TOther right);

    static abstract TResult operator >(TSelf left, TOther right);
}

// A class.
class Money : IEqualityOperators<Money, Money, bool>, IAdditionOperators<Money, Money, Money>,
              IUnaryNegationOperators<Money, Money>, IComparisonOperators<Money, Money, bool>
{
    public int V;

    public static Money Make(int v)
    {
        Money m = new Money();
        m.V = v;
        return m;
    }

    public static bool operator ==(Money a, Money b) { return a.V == b.V; }

    public static bool operator !=(Money a, Money b) { return a.V != b.V; }

    public static Money operator +(Money a, Money b) { return Make(a.V + b.V); }

    public static Money operator -(Money a) { return Make(-a.V); }

    public static bool operator <(Money a, Money b) { return a.V < b.V; }

    public static bool operator >(Money a, Money b) { return a.V > b.V; }
}

// A struct, that has a mixed operator too.
struct Vec : IAdditionOperators<Vec, Vec, Vec>, IAdditionOperators<Vec, int, Vec>,
             IEqualityOperators<Vec, Vec, bool>
{
    public int X;

    public static Vec Make(int x)
    {
        Vec v = new Vec();
        v.X = x;
        return v;
    }

    public static Vec operator +(Vec a, Vec b) { return Make(a.X + b.X); }

    public static Vec operator +(Vec a, int b) { return Make(a.X + b * 10); }

    public static bool operator ==(Vec a, Vec b) { return a.X == b.X; }

    public static bool operator !=(Vec a, Vec b) { return a.X != b.X; }
}

class Util
{
    public static bool Same<T>(T a, T b) where T : IEqualityOperators<T, T, bool>
    {
        return a == b;
    }

    public static bool Diff<T>(T a, T b) where T : IEqualityOperators<T, T, bool>
    {
        return a != b;
    }

    public static T Add<T>(T a, T b) where T : IAdditionOperators<T, T, T>
    {
        return a + b;
    }

    public static T Negate<T>(T a) where T : IUnaryNegationOperators<T, T>
    {
        return -a;
    }

    public static bool Less<T>(T a, T b) where T : IComparisonOperators<T, T, bool>
    {
        return a < b;
    }

    public static bool More<T>(T a, T b) where T : IComparisonOperators<T, T, bool>
    {
        return a > b;
    }

    public static T Sum3<T>(T a, T b, T c) where T : IAdditionOperators<T, T, T>
    {
        T r = a + b;
        r = r + c;
        return r;
    }

    public static Vec AddInt(Vec a, int k)
    {
        return Add2<Vec>(a, k);
    }

    public static T Add2<T>(T a, int k) where T : IAdditionOperators<T, int, T>
    {
        return a + k;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Money a = Money.Make(3);
        Money b = Money.Make(4);

        Console.WriteLine(Util.Same<Money>(a, Money.Make(3)));
        Console.WriteLine(Util.Same<Money>(a, b));
        Console.WriteLine(Util.Diff<Money>(a, Money.Make(3)));
        Console.WriteLine(Util.Diff<Money>(a, b));
        Console.WriteLine(Util.Add<Money>(a, b).V);
        Console.WriteLine(Util.Negate<Money>(a).V);
        Console.WriteLine(Util.Less<Money>(a, b));
        Console.WriteLine(Util.More<Money>(a, b));

        Vec v = Vec.Make(1);
        Vec w = Vec.Make(3);
        Console.WriteLine(Util.Add<Vec>(v, w).X);
        Console.WriteLine(Util.Sum3<Vec>(v, w, Vec.Make(5)).X);
        Console.WriteLine(Util.AddInt(Vec.Make(5), 7).X);
        Console.WriteLine(Util.Same<Vec>(v, Vec.Make(1)));

        Console.WriteLine(Util.Sum3<Money>(Money.Make(1), Money.Make(1), Money.Make(3)).V);
        Console.WriteLine(Util.Add<Money>(Money.Make(5), Money.Make(5)).V);
    }
}
