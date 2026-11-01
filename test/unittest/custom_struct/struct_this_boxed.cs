// The this of a struct is boxed, or passed as a pointer: in a generic struct too.

/* EXPECT

5
6
7
9
7

*/

import System;

using System;

struct S
{
    public int X;

    public Object Box()
    {
        Object o = this;
        return o;
    }
}

struct Point
{
    public int X;

    public int Twice() { return 7; }
}

struct G<T>
{
    public int X;

    public Object Box()
    {
        Object o = this;
        return o;
    }
}

interface IGet<T>
{
    int Get();
}

// A generic struct that implements a generic interface, used as the interface.
struct Impl<T> : IGet<T>
{
    public int Value;

    public int Get() { return Value; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        S s = new S();
        s.X = 5;
        Console.WriteLine(((S)s.Box()).X);

        G<int> g = new G<int>();
        g.X = 6;
        Console.WriteLine(((G<int>)g.Box()).X);

        Impl<String> impl = new Impl<String>();
        impl.Value = 7;
        IGet<String> iget = impl;
        Console.WriteLine(iget.Get());

        // A struct that is made and used at once, without a variable.
        Console.WriteLine(new Point().X + 9);
        Console.WriteLine(new Point().Twice());
    }
}
