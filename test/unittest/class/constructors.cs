// Constructors: the base constructors run, with their arguments; default constructors
// initialize the fields.

/* EXPECT

B
M
D
11
7
5
Base0
3
5
boom

*/

import System;

using System;

class B
{
    public B(int v)
    {
        V = v;
        Console.WriteLine("B");
    }

    public int V;
}

class M : B
{
    public M(int v) : base(v + 1)
    {
        Console.WriteLine("M");
    }
}

class D : M
{
    public D() : base(10)
    {
        Console.WriteLine("D");
    }

    public int W = 7;
}

// No constructor: the compiler makes one, which initializes the field.
class Plain
{
    public int V = 5;
}

class Base0
{
    public Base0()
    {
        Console.WriteLine("Base0");
    }

    public int X = 3;
}

// The default constructor calls the one of the base class.
class Derived0 : Base0
{
}

// A virtual method called by the constructor of the base class.
class VBase
{
    public VBase()
    {
        Init();
    }

    public virtual void Init()
    {
    }
}

class VDerived : VBase
{
    public int V;

    public override void Init()
    {
        V = 5;
    }
}

class MyError : Exception
{
    public MyError(String message) : base(message)
    {
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        D d = new D();
        Console.WriteLine(d.V);
        Console.WriteLine(d.W);

        Plain p = new Plain();
        Console.WriteLine(p.V);

        Derived0 b = new Derived0();
        Console.WriteLine(b.X);

        VDerived v = new VDerived();
        Console.WriteLine(v.V);

        try
        {
            throw new MyError("boom");
        }
        catch (MyError e)
        {
            Console.WriteLine(e.Message);
        }
    }
}
