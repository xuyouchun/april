// (T)o of a generic param T that is a struct, a number, a string or a class: the value that the object is. It is returned, assigned,
// passed, and stored in a field and an array.

/* EXPECT

3 4
3 4
3 4
3 4
3 4
3 4
7
s
x
5 6
a:9
true
invalid cast
cast ok

*/

import System;

using System;

struct P { public Int32 X; public Int32 Y; }

class Pt { public Int32 X; }

class Holder<T>
{
    public T Value;

    public void Set(Object o) { Value = (T)o; }
}

class Program
{
    static T Unbox<T>(Object o)
    {
        return (T)o;
    }

    static T Local<T>(Object o)
    {
        T v = (T)o;
        return v;
    }

    static Int32 Take(P p) { return p.X * 10 + p.Y; }

    static T[] Many<T>(Object a, Object b)
    {
        T[] items = new T[2];
        items[0] = (T)a;
        items[1] = (T)b;
        return items;
    }

    static Boolean Is<T>(Object o)
    {
        try
        {
            T t = (T)o;
            return true;
        }
        catch (InvalidCastException)
        {
            return false;
        }
    }

    [EntryPoint]
    public static void Main()
    {
        P p = new P();
        p.X = 3;
        p.Y = 4;
        Object o = p;

        P r = Unbox<P>(o);
        Console.WriteLine(r.X + " " + r.Y);

        P l = Local<P>(o);
        Console.WriteLine(l.X + " " + l.Y);

        P a;
        a = Unbox<P>(o);
        Console.WriteLine(a.X + " " + a.Y);

        Console.WriteLine(Unbox<P>(o).X + " " + Unbox<P>(o).Y);

        Holder<P> h = new Holder<P>();
        h.Set(o);
        Console.WriteLine(h.Value.X + " " + h.Value.Y);

        P[] items = Many<P>(o, o);
        Console.WriteLine(items[1].X + " " + items[0].Y);

        Console.WriteLine(Unbox<Int32>((Object)7));
        Console.WriteLine(Unbox<String>("s"));

        Holder<String> hs = new Holder<String>();
        hs.Set("x");
        Console.WriteLine(hs.Value);

        Object o2 = new P();
        P q = Unbox<P>(o2);
        q.X = 5;
        q.Y = 6;
        Console.WriteLine(q.X + " " + q.Y);

        Pt pt = new Pt();
        pt.X = 9;
        Console.WriteLine("a:" + Unbox<Pt>(pt).X);

        Console.WriteLine(Is<Int32>((Object)3) ? "true" : "false");
        Console.WriteLine(Is<String>((Object)3) ? "cast ok" : "invalid cast");
        Console.WriteLine(Is<P>(o) ? "cast ok" : "invalid cast");
    }
}
