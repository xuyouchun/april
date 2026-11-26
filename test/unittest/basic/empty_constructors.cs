// A method that does nothing is called without a frame (the interpreter, commands.cpp analyze_trivial_method): the constructor of Object,
// and the constructors that only call the one of the base class, in a chain. A constructor that does something is called as always:
// the field initializers, a body, an argument, a base constructor that does something, a throw. The results are those of the calls.

/* EXPECT

chain 1000 3
init 5 7 12
body 6 3
args 15 4
throws caught 0
struct 4 9
many 100000 300000
generic 7 8

*/

import System;

using System;

class A { }

class B : A { }

class C : B { }

class WithInit : C
{
    public Int32 X = 5;
}

class WithBody : B
{
    public Int32 Y;

    public WithBody() { Y = 6; }
}

class Counter
{
    public static Int32 Built = 0;

    public Counter() { Built++; }
}

class Sub : Counter { }

class SubSub : Sub { }

class WithArgs : C
{
    public Int32 Fa, Fb;

    public WithArgs(Int32 a, Int32 b) { Fa = a; Fb = b; }
}

class Thrower : B
{
    public Thrower() { throw new Exception(); }
}

class Chain1 : Thrower { }

struct Point
{
    public Int32 X;
    public Int32 Y;

    public Point(Int32 x, Int32 y) { X = x; Y = y; }
}

class Box<T>
{
    public T Value;
}

class Box2<T> : Box<T> { }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // The chain C - B - A - Object, many times: the first call parses the constructors, the next ones skip them.
        Int32 made = 0;
        C last = null;
        for (Int32 i = 0; i < 1000; i++)
        {
            last = new C();
            made++;
        }

        Console.WriteLine("chain " + made + " " + (last != null ? 3 : 0));

        WithInit wi = new WithInit();
        WithInit wi2 = new WithInit();
        wi2.X = wi.X + 2;
        Console.WriteLine("init " + wi.X + " " + wi2.X + " " + (wi.X + wi2.X));

        WithBody wb = new WithBody();
        WithBody wb2 = new WithBody();
        Console.WriteLine("body " + wb.Y + " " + (wb2.Y / 2));

        WithArgs wa = new WithArgs(7, 8);
        WithArgs wa2 = new WithArgs(1, 2);
        Console.WriteLine("args " + (wa.Fa + wa.Fb) + " " + (wa2.Fa + wa2.Fb + 1));

        // A base constructor that does something is run for the derived classes that do nothing.
        Counter.Built = 0;
        Sub s1 = new Sub();
        SubSub s2 = new SubSub();
        SubSub s3 = new SubSub();
        for (Int32 i = 0; i < 3; i++)
            new SubSub();

        String r = "none";
        try { Chain1 c = new Chain1(); r = "made"; }
        catch (Exception) { r = "caught"; }

        try { Chain1 c = new Chain1(); r = "made"; }
        catch (Exception) { r = "caught"; }

        Console.WriteLine("throws " + r + " " + (Counter.Built - 6));

        Point p = new Point(4, 9);
        Point q = new Point();
        Console.WriteLine("struct " + p.X + " " + (p.Y + q.X));

        Int64 total = 0;
        for (Int32 i = 0; i < 100000; i++)
        {
            A a = new A();
            B b = new B();
            C c = new C();
            total += 3;
        }

        Console.WriteLine("many " + (total / 3) + " " + total);

        Box2<Int32> b2 = new Box2<Int32>();
        b2.Value = 7;
        Box2<Int32> b3 = new Box2<Int32>();
        b3.Value = b2.Value + 1;
        Console.WriteLine("generic " + b2.Value + " " + b3.Value);
    }
}
