// A value of a generic parameter is an object: the cast to Object and the conversion to Object
// give the reference, or box a struct, a number, a string, an enum.

/* EXPECT

True
False
18
7
42
5000000000
hi
True
True
9

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
    public int N;
}

enum Color { Red, Green }

class Util
{
    public static bool IsNull<T>(T x)
    {
        Object o = (Object)x;
        return o == null;
    }

    public static Object Boxed<T>(T x)
    {
        Object o = x;
        return o;
    }

    public static Object Direct<T>(T x)
    {
        return (Object)x;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box nothing = null;
        Console.WriteLine(Util.IsNull<Box>(nothing));
        Console.WriteLine(Util.IsNull<Box>(new Box()));

        P p = new P();
        p.V = 5;
        p.W = 6;
        p.Z = 7;

        P q = (P)Util.Boxed<P>(p);
        Console.WriteLine(q.V + q.W + q.Z);
        Console.WriteLine(((P)Util.Direct<P>(p)).Z);

        Console.WriteLine((int)Util.Boxed<int>(42));
        Console.WriteLine((long)Util.Direct<long>(5000000000));
        Console.WriteLine((String)Util.Boxed<String>("hi"));
        Console.WriteLine((Color)Util.Boxed<Color>(Color.Green) == Color.Green);
        Console.WriteLine(Util.IsNull<String>(null));

        Box box = new Box();
        box.N = 9;
        Console.WriteLine(((Box)Util.Direct<Box>(box)).N);
    }
}
