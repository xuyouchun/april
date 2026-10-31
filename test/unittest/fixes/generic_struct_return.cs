// Generic params returned by value, when the argument is a custom struct.

/* EXPECT

7
4
3
4
5
9
8

*/

import System;

using System;

struct P
{
    public int X;
    public int Y;
}

class Box<T>
{
    public T V;

    public Box(T v) { V = v; }

    public T Get() { return V; }

    public T Id(T a) { return a; }
}

class Util
{
    public static T Identity<T>(T v) { return v; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        P p = new P();
        p.X = 3;
        p.Y = 4;

        Box<P> b = new Box<P>(p);

        P q = b.Get();
        Console.WriteLine(q.X + q.Y);

        P r = b.Id(p);
        Console.WriteLine(r.Y);

        P s = Util.Identity<P>(p);
        Console.WriteLine(s.X);

        // The result is used without a variable.
        Console.WriteLine(b.Get().Y);

        // Different arguments of the same generic type.
        Box<int> bi = new Box<int>(5);
        Console.WriteLine(bi.Get());

        Box<long> bl = new Box<long>(9);
        Console.WriteLine(bl.Get());

        // A struct in a box that is a field of another.
        P other = new P();
        other.X = 8;
        Box<P> b2 = new Box<P>(other);
        Console.WriteLine(b2.Get().X);
    }
}
