// Generic params returned by value: fields, arguments, locals.

/* EXPECT

7
x
8
3
7
5
9
2.5
True
5000000000
hello

*/

import System;

using System;

class Box<T>
{
    public T V;

    public Box(T v) { V = v; }

    public T Get() { return V; }

    public T Id(T a) { return a; }

    public T ViaLocal() { T t = V; return t; }
}

class Util
{
    public static T Identity<T>(T value) { return value; }

    public static T Pick<T>(bool first, T a, T b)
    {
        if (first)
            return a;

        return b;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box<int> b = new Box<int>(7);
        Console.WriteLine(b.Get());

        Box<String> s = new Box<String>("x");
        Console.WriteLine(s.Get());

        int x = b.Get() + 1;
        Console.WriteLine(x);

        Console.WriteLine(b.Id(3));
        Console.WriteLine(b.ViaLocal());

        Console.WriteLine(Util.Identity<int>(5));
        Console.WriteLine(Util.Pick<int>(false, 1, 9));

        Box<double> d = new Box<double>(2.5);
        Console.WriteLine(d.Get());

        Box<bool> f = new Box<bool>(true);
        Console.WriteLine(f.Get());

        Box<long> l = new Box<long>(5000000000);
        Console.WriteLine(l.Get());

        Console.WriteLine(Util.Identity<String>("hello"));
    }
}
