// The inlining of calls (modules/exec/parser.cpp, __expand_calls) of small methods that have jumps and several returns: an if, a loop,
// a conditional expression, a ret in the middle (it becomes a jump to the end of the body), a throw. The results are those of the calls
// (APRIL_INLINE_CALLS=0 gives the same ones), whatever APRIL_SUPER is. The body can be a loop of its own, read args that the caller
// pushed just before the call, be called in a loop, in a try block, and by a method that has a switch.

/* EXPECT

max 5 9 -3 7
abs 5 5 0 12
clamp 0 10 5 -1
sign -1 0 1
sum 55 0 5050
loop 10 24 15
list 10 40 caught caught 99
chain 3 7 12
throws caught 11 caught 0
nested 12 3
blocks 12 caught 5
switch 4 5 6 0
hot 89147
generic 4 04

*/

import System;

using System;
using System.Collections;

class Box
{
    public Int32 V;

    public Box(Int32 v) { V = v; }

    public Int32 AtLeast(Int32 low) { if (V < low) return low; return V; }

    public Int32 Dist(Int32 other) { return V > other ? V - other : other - V; }
}

class Program
{
    static Int32 Max(Int32 a, Int32 b) { if (a > b) return a; return b; }

    static Int32 Abs(Int32 a) { return a < 0 ? -a : a; }

    static Int32 Clamp(Int32 v, Int32 low, Int32 high)
    {
        if (v < low)
            return low;

        if (v > high)
            return high;

        return v;
    }

    static Int32 Sign(Int32 v)
    {
        if (v < 0)
            return -1;

        if (v == 0)
            return 0;

        return 1;
    }

    static Int32 SumTo(Int32 n)
    {
        Int32 s = 0;
        for (Int32 i = 1; i <= n; i++)
            s += i;

        return s;
    }

    static Int32 Count(Int32 n, Int32 step)
    {
        Int32 c = 0;
        while (n > 0)
        {
            n -= step;
            c++;
        }

        return c;
    }

    static Int32 Check(Int32 v)
    {
        if (v < 0)
            throw new Exception();

        return v + 1;
    }

    static Int32 Pick(Int32 k) { return k == 1 ? 3 : k == 2 ? 7 : 12; }

    static Int32 Twice(Int32 a) { return Max(a, 0) + Abs(a); }

    static Int32 Switchy(Int32 x)
    {
        switch (x)
        {
            case 1: return Max(x, 3) + 1;
            case 2: return Max(x, 5);
            case 3: return Abs(-x) + 3;
            default: return 0;
        }
    }

    static Int32 InBlocks(Int32 v)
    {
        Int32 r = 0;
        try
        {
            r = Check(v) + Clamp(v, 0, 5);
        }
        catch (Exception)
        {
            r = -100;
        }
        finally
        {
            r += 1;
        }

        return r;
    }

    static T Pass<T>(T value, Boolean yes) { if (yes) return value; return default(T); }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("max " + Max(3, 5) + " " + Max(9, 2) + " " + Max(-3, -3) + " " + Max(Max(1, 7), Max(2, 3)));

        Int32 neg = -5;
        Console.WriteLine("abs " + Abs(neg) + " " + Abs(5) + " " + Abs(0) + " " + (Abs(neg) + Abs(-7)));

        Console.WriteLine("clamp " + Clamp(-4, 0, 10) + " " + Clamp(40, 0, 10) + " " + Clamp(5, 0, 10) + " " + Clamp(-1, -1, -1));

        Console.WriteLine("sign " + Sign(-9) + " " + Sign(0) + " " + Sign(33));

        Console.WriteLine("sum " + SumTo(10) + " " + SumTo(0) + " " + SumTo(100));

        Console.WriteLine("loop " + Count(30, 3) + " " + Count(70, 3) + " " + Count(15, 1));

        // A list: the indexer checks the index (a jump and a throw).
        List<Int32> list = new List<Int32>();
        for (Int32 i = 0; i < 5; i++)
            list.Add(i * 10);

        String t1 = "none", t2 = "none";
        try { Int32 x = list[9]; t1 = "read " + x; }
        catch (Exception) { t1 = "caught"; }

        try { Int32 x = list[-1]; t2 = "read " + x; }
        catch (Exception) { t2 = "caught"; }

        Console.WriteLine("list " + (list[1] + 0) + " " + list[4] + " " + t1 + " " + t2 + " " + (list[2] + list[3] + 49));

        Box box = new Box(7);
        Console.WriteLine("chain " + Pick(1) + " " + Pick(2) + " " + (Pick(3) + 0));

        String r1 = "none", r2 = "none";
        for (Int32 k = 0; k < 3; k++)
        {
            try { Int32 c = Check(-1); r1 = "made " + c; }
            catch (Exception) { r1 = "caught"; }
        }

        try { Int32 c = Check(5); r2 = "made " + c; }
        catch (Exception) { r2 = "caught"; }

        Box none = null;
        String r3 = "none";
        try { Int32 c = none.AtLeast(3); r3 = "got " + c; }
        catch (NullReferenceException) { r3 = "caught"; }

        Console.WriteLine("throws " + r1 + " " + (Check(10)) + " " + r3 + " " + (box.AtLeast(3) - 7) + (r2 == "made 6"? "" : "x"));

        Console.WriteLine("nested " + Twice(6) + " " + (Twice(-3) + Max(0, 0)));

        Console.WriteLine("blocks " + (InBlocks(3) + 4) + " " + (InBlocks(-4) < 0? "caught" : "x") + " " + (InBlocks(1) + 2 - 1));

        Console.WriteLine("switch " + Switchy(1) + " " + Switchy(2) + " " + Switchy(3) + " " + Switchy(4));

        Int32 acc = 0;
        for (Int32 i = 0; i < 1000; i++)
            acc += Clamp(i, 10, 90) + Sign(i - 500) + Max(i % 7, 3) - Abs(i % 3 - 1) + box.Dist(i % 11) - box.Dist(i % 11);

        Console.WriteLine("hot " + acc);

        Console.WriteLine("generic " + Pass<Int32>(4, true) + " " + Pass<Int32>(3, false) + Pass<Int32>(4, true));
    }
}
