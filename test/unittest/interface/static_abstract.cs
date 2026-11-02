// The static members of an interface: abstract (a type that implements the interface has them)
// and virtual (they have a default, that a type can replace); they are used through a generic
// parameter that has the interface as a constraint: T.MaxValue, T.Make(1).

/* EXPECT

100
-100
7
default
custom
Small
inherited
12
5
42
42

*/

import System;

using System;

interface IMinMaxValue<TSelf> where TSelf : IMinMaxValue<TSelf>
{
    static abstract TSelf MaxValue { get; }

    static abstract TSelf MinValue { get; }

    static abstract TSelf Make(int x);

    static abstract String Name();

    // The default, that is replaced by what a type has.
    static virtual String Describe() { return "default"; }
}

class Small : IMinMaxValue<Small>
{
    public int V;

    public static Small MaxValue { get { return Make(100); } }

    public static Small MinValue { get { return Make(-100); } }

    public static Small Make(int x)
    {
        Small s = new Small();
        s.V = x;
        return s;
    }

    public static String Name() { return "Small"; }
}

class Custom : IMinMaxValue<Custom>
{
    public int V;

    public static Custom MaxValue { get { return Make(1); } }

    public static Custom MinValue { get { return Make(0); } }

    public static Custom Make(int x)
    {
        Custom c = new Custom();
        c.V = x;
        return c;
    }

    public static String Name() { return "Custom"; }

    public static String Describe() { return "custom"; }
}

interface IZero
{
    static abstract int Zero();
}

class Base
{
    public static int Zero() { return 12; }
}

class Inherited : Base, IZero { }

class Util
{
    public static int Largest<T>() where T : class, IMinMaxValue<T>
    {
        return ((Small)(Object)T.MaxValue).V;
    }

    public static T Smallest<T>() where T : IMinMaxValue<T>
    {
        return T.MinValue;
    }

    public static T Make<T>(int x) where T : IMinMaxValue<T>
    {
        return T.Make(x);
    }

    public static String Describe<T>() where T : IMinMaxValue<T>
    {
        return T.Describe();
    }

    public static String Name<T>() where T : IMinMaxValue<T>
    {
        return T.Name();
    }

    public static int Zero<T>() where T : IZero
    {
        return T.Zero();
    }
}

// A generic class, the constraint of its parameter.
class Box<T> where T : IZero
{
    public int Get() { return T.Zero(); }

    public static int GetStatic() { return T.Zero(); }
}

class Forty : IZero
{
    public static int Zero() { return 42; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Util.Largest<Small>());
        Console.WriteLine(Util.Smallest<Small>().V);
        Console.WriteLine(Util.Make<Small>(7).V);
        Console.WriteLine(Util.Describe<Small>());
        Console.WriteLine(Util.Describe<Custom>());
        Console.WriteLine(Util.Name<Small>());
        Console.WriteLine("inherited");
        Console.WriteLine(Util.Zero<Inherited>());
        Console.WriteLine(Util.Make<Custom>(5).V);

        Console.WriteLine(new Box<Forty>().Get());
        Console.WriteLine(Box<Forty>.GetStatic());
    }
}
