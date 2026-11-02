// The instance members of the constraints of a generic parameter are called through it:
// t.Get(), t.Value, t.Value = 7, where T : IFoo. A class takes the virtual table, a struct is
// the object itself (it is changed by the call).

/* EXPECT

5
25
7
4
22
7
99
99
100
100
5

*/

import System;

using System;

interface IFoo
{
    int Get();

    int Value { get; set; }

    int Add(int x);
}

class A : IFoo
{
    public int N;

    public int Get() { return N; }

    public int Value { get { return N * 2; } set { N = value; } }

    public int Add(int x) { return N + x; }
}

struct S : IFoo
{
    public int N;

    public int Get() { return N; }

    public int Value { get { return N * 2; } set { N = value; } }

    public int Add(int x) { return N + x; }
}

// A class that implements the interface with a virtual method, and one that derives.
class Shape
{
    public virtual int Area() { return 10; }
}

class Big : Shape
{
    public override int Area() { return 99; }
}

interface IArea
{
    int Area();
}

class Square : IArea
{
    public virtual int Area() { return 100; }
}

class Cube : Square
{
    public override int Area() { return 100; }
}

class Util
{
    public static int Call<T>(T t) where T : IFoo
    {
        return t.Get();
    }

    public static int Props<T>(T t) where T : IFoo
    {
        return t.Value + t.Add(10);
    }

    public static int SetVal<T>(T t) where T : IFoo
    {
        t.Value = 7;
        return t.Get();
    }

    // A base class as a constraint, the method is virtual.
    public static int Area<T>(T t) where T : Shape
    {
        return t.Area();
    }

    public static int Interface<T>(T t) where T : IArea
    {
        return t.Area();
    }

    public static int Sum<T>(T t) where T : IFoo
    {
        return t.Add(1) + t.Add(2) + t.Get() * 2 - 3 + t.Value - t.Get() * 2 + 1;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        A a = new A();
        a.N = 5;
        Console.WriteLine(Util.Call<A>(a));
        Console.WriteLine(Util.Props<A>(a));
        Console.WriteLine(Util.SetVal<A>(a));

        S s = new S();
        s.N = 4;
        Console.WriteLine(Util.Call<S>(s));
        Console.WriteLine(Util.Props<S>(s));
        Console.WriteLine(Util.SetVal<S>(s));

        Console.WriteLine(Util.Area<Shape>(new Big()));
        Console.WriteLine(Util.Area<Big>(new Big()));
        Console.WriteLine(Util.Interface<Square>(new Cube()));
        Console.WriteLine(Util.Interface<Cube>(new Cube()));

        S t = new S();
        t.N = 1;
        Console.WriteLine(Util.Sum<S>(t));
    }
}
