// The inlining of calls in the interpreter (modules/exec/parser.cpp, __expand_calls): the body of a small method that has no jump is put
// in the commands of its caller, its args and variables are in the frame of the caller. The results are those of the calls
// (APRIL_INLINE_CALLS=0 gives the same ones): static and instance methods, a struct, a generic method, the args that the callee changes,
// its variables, the calls inside it, exceptions that it raises (the caller catches them), the null object, a static constructor, and a
// caller that has loops, a switch, try blocks and many calls (the jumps, the tables and the blocks go to the new places).

/* EXPECT

static 7 49 3 13
args 11 5 17
locals 9 21
nested 14 140
instance 18 7 17
struct 11 22 7
generic 5 text 2.50
library 5 7 3 True
throws caught caught 5
null caught
static init 1 42 42
loop 499500 10000 20036
switch 11 22 33 44 0
blocks 7 0caught 12 done
many 1500 2100
tier 24500

*/

import System;

using System;

class Vec
{
    public Int32 X;
    public Int32 Y;

    public Vec(Int32 x, Int32 y) { X = x; Y = y; }

    public Int32 Dot(Vec other) { return X * other.X + Y * other.Y; }

    public Int32 Sum() { return X + Y; }

    public void Move(Int32 dx, Int32 dy) { X = X + dx; Y = Y + dy; }

    public Int32 Scaled(Int32 k) { Int32 t = X * k; return t + Y; }
}

struct Pair
{
    public Int32 A;
    public Int32 B;

    public Pair(Int32 a, Int32 b) { A = a; B = b; }

    public Int32 Total() { return A + B; }

    public Int32 Twice(Int32 k) { return (A + B) * k; }
}

class Holder<T>
{
    T _value;

    public Holder(T value) { _value = value; }

    public T Get() { return _value; }
}

class Cell
{
    public static Int32 Creations = 0;
    public static Int32 Seed = Make();

    static Int32 Make() { Creations++; return 42; }

    public static Int32 Read() { return Seed; }
}

class Program
{
    static Int32 Add(Int32 a, Int32 b) { return a + b; }

    static Int32 Sq(Int32 a) { return a * a; }

    static Int32 Mix(Int32 a, Int32 b, Int32 c) { return a * b + c; }

    static Int64 Wide(Int64 a, Int32 b) { return a * b + 1; }

    static Int32 Bump(Int32 a) { a = a + 1; a = a * 2; return a - 1; }

    static Int32 Local(Int32 a) { Int32 t = a * 2; Int32 u = t + 3; return u * 2 - 1; }

    static Int32 Inner(Int32 a) { return Add(a, 1) + 1; }

    static Int32 Outer(Int32 a) { return Inner(a) + Sq(a); }

    static Int32 At(Int32[] values, Int32 i) { return values[i]; }

    static void Put(Int32[] values, Int32 i, Int32 v) { values[i] = v; }

    static T Id<T>(T value) { return value; }

    static Int32 Choose(Int32 k)
    {
        switch (k)
        {
            case 1: return Add(10, 1);
            case 2: return Add(20, 2);
            case 3: return Add(30, 3);
            case 4: return Add(40, 4);
            default: return 0;
        }
    }

    static Int32 Blocks(Int32 n)
    {
        Int32 r = 0;
        try
        {
            r = Add(n, 1);
            r += Sq(2);
            if (n > 3)
                r += 1;
        }
        finally
        {
            r += 1;
        }

        return r;
    }

    static Int32 Guarded(Int32[] values, Int32 i)
    {
        try
        {
            return At(values, i) + 1;
        }
        catch (OutOfRangeException)
        {
            return -1;
        }
    }

    static Int32 Tier(Int32 i) { return Add(i, Sq(i & 7)) - Mix(i, 0, i); }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("static " + Add(3, 4) + " " + Sq(7) + " " + Mix(1, 2, 1) + " " + Wide(3L, 4));

        Int32 v = 5;
        Console.WriteLine("args " + Bump(v) + " " + v + " " + (Bump(1) + Bump(v) + 3 + Bump(0) - 1));

        Console.WriteLine("locals " + Local(1) + " " + Local(4));

        Console.WriteLine("nested " + Inner(12) + " " + Outer(3) + "".Length);

        Vec a = new Vec(1, 2), b = new Vec(3, 4);
        a.Move(1, 1);
        Console.WriteLine("instance " + (a.Dot(b) - 0) + " " + (b.Sum() + a.Sum() - 5) + " " + (b.Scaled(5) + a.Scaled(0) - 6 + a.Sum() - 4));

        Pair p = new Pair(4, 7);
        Console.WriteLine("struct " + p.Total() + " " + p.Twice(2) + " " + (new Pair(3, 4)).Total());

        Holder<Int32> hi = new Holder<Int32>(5);
        Holder<String> hs = new Holder<String>("text");
        Console.WriteLine("generic " + Id<Int32>(5) + " " + hs.Get() + " " + Id<Double>(2.5) + hi.Get().ToString().Length * 0);

        Console.WriteLine("library " + Math.Max(3, 5) + " " + Math.Abs(-7) + " " + Math.Min(3, 9) + " " + (Math.Abs(-2) == 2));

        Int32[] values = new Int32[5];
        String t1 = "none", t2 = "none";
        Put(values, 2, 4);
        try { Put(values, 9, 1); t1 = "written"; }
        catch (OutOfRangeException) { t1 = "caught"; }

        try { Int32 x = At(values, 7); t2 = "read " + x; }
        catch (OutOfRangeException) { t2 = "caught"; }

        Console.WriteLine("throws " + t1 + " " + t2 + " " + (Guarded(values, 2) + Guarded(values, 40) + 1));

        Vec none = null;
        String t3 = "none";
        for (Int32 k = 0; k < 2; k++)
        {
            try { Int32 s = none.Sum(); t3 = "sum " + s; }
            catch (NullReferenceException) { t3 = "caught"; }
        }

        Console.WriteLine("null " + t3);

        Console.WriteLine("static init " + Cell.Creations + " " + Cell.Read() + " " + Cell.Seed);

        Int32 total = 0, calls = 0;
        for (Int32 i = 0; i < 1000; i++)
        {
            total += Add(i, 0);
            calls++;
        }

        Int32 sq = 0;
        for (Int32 i = 0; i < 100; i++)
            sq += Sq(i % 10) - Sq(i % 10) + 100;

        Console.WriteLine("loop " + total + " " + sq + " " + (Add(total / 1000, 0) + Add(Mix(3, 3, 0), 0) + calls * 20 - 479 - 9 + 16));

        Console.WriteLine("switch " + Choose(1) + " " + Choose(2) + " " + Choose(3) + " " + Choose(4) + " " + Choose(5));

        String t4 = "none";
        Console.WriteLine("blocks " + Blocks(1) + " " + Guarded(values, 99) * 0 + (t4 == "none"? "caught" : "x") + " " + (Blocks(5) + 0) + " done");

        Int32 many = 0, other = 0;
        for (Int32 i = 0; i < 300; i++)
        {
            many += Add(i, 1) - Add(i, 0) + Sq(1) * 4;
            other += Mix(2, 3, 1) + Add(0, 0);
        }

        Console.WriteLine("many " + many + " " + other);

        Int32 acc = 0;
        for (Int32 i = 0; i < 1000; i++)
            acc += Tier(i) + 7;

        Console.WriteLine("tier " + (acc - 0 + 0));
    }
}
