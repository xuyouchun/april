// The accessors of the properties (a method that only reads or writes a field of this) are called without a frame (the interpreter,
// commands.cpp analyze_trivial_method). The results are those of the calls: every type of field, class and struct, a generic class,
// a property that does more than the access, the call of a null object, the methods that override (they are virtual: a real call).
// APRIL_INLINE_ACCESSORS=0 gives the same results.

/* EXPECT

types 1 2 3 4 5 6 7 8 1.5 2.5 x True s o
auto 41 42 42
struct 5 15 16
generic 7 hello 3.5
more 3 6
virtual 10 22 11
null caught caught
loop 499500 1000 25
static 9

*/

import System;

using System;

class Types
{
    Int8 _a; UInt8 _b; Int16 _c; UInt16 _d; Int32 _e; UInt32 _f; Int64 _g; UInt64 _h;
    Single _i; Double _j; Char _k; Boolean _l; String _m; Object _n;

    public Int8 A { get { return _a; } set { _a = value; } }
    public UInt8 B { get { return _b; } set { _b = value; } }
    public Int16 C { get { return _c; } set { _c = value; } }
    public UInt16 D { get { return _d; } set { _d = value; } }
    public Int32 E { get { return _e; } set { _e = value; } }
    public UInt32 F { get { return _f; } set { _f = value; } }
    public Int64 G { get { return _g; } set { _g = value; } }
    public UInt64 H { get { return _h; } set { _h = value; } }
    public Single I { get { return _i; } set { _i = value; } }
    public Double J { get { return _j; } set { _j = value; } }
    public Char K { get { return _k; } set { _k = value; } }
    public Boolean L { get { return _l; } set { _l = value; } }
    public String M { get { return _m; } set { _m = value; } }
    public Object N { get { return _n; } set { _n = value; } }
}

class Auto
{
    public Int32 X { get; set; }
    public Int32 Y { get; set; }
}

struct Pair
{
    Int32 _a;
    Int32 _b;

    public Int32 A { get { return _a; } set { _a = value; } }
    public Int32 B { get { return _b; } set { _b = value; } }
}

class Cell<T>
{
    T _value;

    public T Value { get { return _value; } set { _value = value; } }
}

class Counted
{
    Int32 _v;
    public Int32 Reads;

    public Int32 V
    {
        get { Reads++; return _v; }
        set { _v = value + 1; }
    }
}

class Base
{
    protected Int32 _x = 10;

    public virtual Int32 X { get { return _x; } set { _x = value; } }
}

class Derived : Base
{
    public override Int32 X { get { return _x * 2; } set { _x = value + 1; } }
}

class Program
{
    static Int32 _s = 9;

    static Int32 S { get { return _s; } }

    [EntryPoint]
    public static void Main()
    {
        Types t = new Types();
        t.A = 1; t.B = 2; t.C = 3; t.D = 4; t.E = 5; t.F = 6; t.G = 7; t.H = 8;
        t.I = 1.5f; t.J = 2.5; t.K = 'x'; t.L = true; t.M = "s"; t.N = "o";

        // The first call of an accessor parses it, the next ones are the fast ones: call each one twice.
        Int32 sum = 0;
        for (Int32 r = 0; r < 2; r++)
        {
            sum += t.A + t.B + t.C + t.D + t.E + (Int32)t.F + (Int32)t.G + (Int32)t.H;
            t.A = t.A; t.M = t.M;
        }

        Console.WriteLine("types " + t.A + " " + t.B + " " + t.C + " " + t.D + " " + t.E + " " + t.F + " " + t.G + " " + t.H + " " + t.I + " " + t.J
                          + " " + t.K + " " + (t.L ? "True" : "False") + " " + t.M + " " + t.N);

        Auto a = new Auto();
        a.X = 40; a.Y = 41;
        a.X = a.X + 1; a.Y = a.Y + a.X - 40;
        a.X++;
        Console.WriteLine("auto " + (a.X - 1) + " " + a.Y + " " + a.X);

        Pair p = new Pair();
        p.A = 5; p.B = 10;
        p.B += p.A;
        Pair q = p;
        q.A = 16;
        Console.WriteLine("struct " + p.A + " " + p.B + " " + q.A);

        Cell<Int32> ci = new Cell<Int32>();
        Cell<String> cs = new Cell<String>();
        Cell<Double> cd = new Cell<Double>();
        ci.Value = 7; cs.Value = "hello"; cd.Value = 3.5;
        for (Int32 i = 0; i < 2; i++)
            ci.Value = ci.Value;
        Console.WriteLine("generic " + ci.Value + " " + cs.Value + " " + cd.Value);

        Counted c = new Counted();
        c.V = 1;
        Int32 v1 = c.V, v2 = c.V + 3;
        Console.WriteLine("more " + (c.Reads + 1) + " " + (v1 + v2 - 1));

        Base b = new Base();
        Base d = new Derived();
        b.X = 10; d.X = 10;
        Console.WriteLine("virtual " + b.X + " " + d.X + " " + ((Derived)d).X / 2);

        Types none = null;
        Auto no = null;
        String r1 = "none", r2 = "none";
        for (Int32 k = 0; k < 2; k++)
        {
            try { Int32 x = no.X; r1 = "read " + x; }
            catch (NullReferenceException) { r1 = "caught"; }

            try { no.X = 3; r2 = "written"; }
            catch (NullReferenceException) { r2 = "caught"; }
        }

        Console.WriteLine("null " + r1 + " " + r2);

        Auto h = new Auto();
        Int32 total = 0;
        for (Int32 i = 0; i < 1000; i++)
        {
            h.X = i;
            total += h.X;
            h.Y = h.Y + 1;
        }

        Console.WriteLine("loop " + total + " " + h.Y + " " + (h.X + 1 - 975));

        Console.WriteLine("static " + S);
    }
}
