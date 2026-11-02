// A generic method returns the default value, a field, a property or the result of a method of
// a generic type, when the argument is a struct, a number or a class.

/* EXPECT

0
0
0
0
0
True
True
18
18
18
7
hello
9
True

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

class Holder<T>
{
    public T Value;

    public T Get() { return Value; }

    public T Prop { get { return Value; } }
}

class Util
{
    public static T Make<T>() { return default(T); }

    public static T Id<T>(T x) { return x; }

    // The default value is an argument, a local variable, a member.
    public static T Arg<T>() { return Id<T>(default(T)); }

    public static T Local<T>() { T y = default(T); return y; }

    public static T FromField<T>(Holder<T> h) { return h.Value; }

    public static T FromMethod<T>(Holder<T> h) { return h.Get(); }

    public static T FromProp<T>(Holder<T> h) { return h.Prop; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        P a = Util.Make<P>();
        Console.WriteLine(a.V + a.W + a.Z);

        P b = Util.Arg<P>();
        Console.WriteLine(b.V + b.W + b.Z);

        P c = Util.Local<P>();
        Console.WriteLine(c.V + c.W + c.Z);

        Console.WriteLine(Util.Make<int>());
        Console.WriteLine(Util.Arg<long>());
        Console.WriteLine(Util.Make<Box>() == null);
        Console.WriteLine(Util.Make<String>() == null);

        P p = new P();
        p.V = 5;
        p.W = 6;
        p.Z = 7;

        Holder<P> h = new Holder<P>();
        h.Value = p;

        P f = Util.FromField<P>(h);
        P m = Util.FromMethod<P>(h);
        P r = Util.FromProp<P>(h);
        Console.WriteLine(f.V + f.W + f.Z);
        Console.WriteLine(m.V + m.W + m.Z);
        Console.WriteLine(r.V + r.W + r.Z);

        Holder<int> hi = new Holder<int>();
        hi.Value = 7;
        Console.WriteLine(Util.FromProp<int>(hi));

        Holder<Box> hb = new Holder<Box>();
        hb.Value = new Box();
        hb.Value.Name = "hello";
        Console.WriteLine(Util.FromMethod<Box>(hb).Name);

        Console.WriteLine(Util.FromField<P>(h).Z + Util.FromMethod<P>(h).W - 4);
        Console.WriteLine(Util.FromProp<Box>(hb).Name == "hello");
    }
}
