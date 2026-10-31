// Delegates that take and return custom structs.

/* EXPECT

6
3
7
11
hi
300
2

*/

import System;

using System;

struct P
{
    public int X;
    public int Y;
}

class H
{
    public int Base;

    public static P Mk(int a)
    {
        P p = new P();
        p.X = a;
        p.Y = a * 2;
        return p;
    }

    public static int Sum(P p) { return p.X + p.Y; }

    public static P Add(P a, P b)
    {
        P r = new P();
        r.X = a.X + b.X;
        r.Y = a.Y + b.Y;
        return r;
    }

    public P Shift(int d)
    {
        P p = new P();
        p.X = Base + d;
        p.Y = Base - d;
        return p;
    }

    public static void Say(String s) { Console.WriteLine(s); }
    public static int Mul(int a, int b) { return a * b; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Delegate<P, int> mk = H.Mk;
        P r = mk(3);
        Console.WriteLine(r.Y);

        Delegate<int, P> sum = H.Sum;
        P q = new P();
        q.X = 1;
        q.Y = 2;
        Console.WriteLine(sum(q));

        Delegate<P, P, P> add = H.Add;
        P s = add(r, q);
        Console.WriteLine(s.X + 3);

        H h = new H();
        h.Base = 10;
        Delegate<P, int> shift = h.Shift;
        P t = shift(1);
        Console.WriteLine(t.X);

        Delegate<Void, String> say = H.Say;
        say("hi");

        Delegate<int, int, int> mul = H.Mul;
        Console.WriteLine(mul(100, 3));

        // The result of a delegate is used again.
        Console.WriteLine(mk(mk(1).Y).X);
    }
}
