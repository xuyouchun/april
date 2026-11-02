// A generic method returns a struct, through the other generic methods that return it:
// return Id<T>(x). The struct is written where the caller wants it, when T is one.

/* EXPECT

18
18
18
18
7
hello
24
100

*/

import System;

using System;

struct P
{
    public int V;
    public int W;
    public int Z;
}

class Box
{
    public String Name;
}

interface IBest<TSelf> where TSelf : IBest<TSelf>
{
    static abstract TSelf Best { get; }
}

struct Level : IBest<Level>
{
    public int V;

    public static Level Best
    {
        get
        {
            Level l = new Level();
            l.V = 100;
            return l;
        }
    }
}

class Util
{
    public static T Id<T>(T x) { return x; }

    // Returns what the other generic method returns.
    public static T Twice<T>(T x) { return Id<T>(x); }

    public static T Thrice<T>(T x) { return Twice<T>(x); }

    public static T Local<T>(T x) { T y = Id<T>(x); return y; }

    public static T Best<T>() where T : IBest<T> { return T.Best; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        P p = new P();
        p.V = 5;
        p.W = 6;
        p.Z = 7;

        P a = Util.Twice<P>(p);
        Console.WriteLine(a.V + a.W + a.Z);

        P b = Util.Thrice<P>(p);
        Console.WriteLine(b.V + b.W + b.Z);

        P c = Util.Local<P>(p);
        Console.WriteLine(c.V + c.W + c.Z);

        // The destination is a member of an expression.
        Console.WriteLine(Util.Twice<P>(p).V + Util.Twice<P>(p).W + Util.Twice<P>(p).Z);

        // Not a struct: the value is in the stack.
        Console.WriteLine(Util.Thrice<int>(7));

        Box box = new Box();
        box.Name = "hello";
        Console.WriteLine(Util.Thrice<Box>(box).Name);

        Console.WriteLine(Util.Twice<P>(p).Z + Util.Twice<P>(p).W + Util.Twice<P>(p).V + 6);

        // A getter of an interface, called through the parameter.
        Console.WriteLine(Util.Best<Level>().V);
    }
}
