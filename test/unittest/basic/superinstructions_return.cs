// The superinstructions of the return (modules/exec/fusion.h, ret_push and the operations that end with a ret): the value that a method
// returns is a local, an argument, a constant, or the result of an operation; the types; the methods that return a struct, an object, a
// boolean, nothing (they are not changed, or have their own command); the recursion; the methods that run many times (they get their
// commands after 50 calls). The results are those of the commands one by one (APRIL_SUPER=off), whatever the mode is.

/* EXPECT

values 5 9 7 123456789012 2.5 1.5 x True
ops 12 -2 35 3 1 1 8 5
compare True False True False True
wide 5000000000 90000000000 2.75
recursion 832040 3628800 55
object a b
struct 3 4 25
void 3 99
args 7 -1 4 2.5
mixed 300 29
tier 1500 499500 true

*/

import System;

using System;

struct Point
{
    public Int32 X;
    public Int32 Y;

    public Point(Int32 x, Int32 y) { X = x; Y = y; }
}

class Box
{
    public Int32 V;

    public Box(Int32 v) { V = v; }

    public Int32 Get() { return V; }

    public Int32 Twice() { return V * 2; }

    public Box Self() { return this; }
}

class Program
{
    static Int32 ArgI(Int32 a) { return a; }

    static Int32 ConstI() { return 9; }

    static Int32 LocalI(Int32 a) { Int32 t = a + 2; return t; }

    static Int64 ArgL(Int64 a) { return a; }

    static Double ArgD(Double a) { return a; }

    static Single ArgF(Single a) { return a; }

    static Char ArgC(Char a) { return a; }

    static Boolean ArgB(Boolean a) { return a; }

    static Int32 Add(Int32 a, Int32 b) { return a + b; }

    static Int32 Sub(Int32 a, Int32 b) { return a - b; }

    static Int32 Mul(Int32 a, Int32 b) { return a * b; }

    static Int32 Div(Int32 a, Int32 b) { return a / b; }

    static Int32 Mod(Int32 a, Int32 b) { return a % b; }

    static Int32 And(Int32 a, Int32 b) { return a & b; }

    static Int32 Shl(Int32 a, Int32 b) { return a << b; }

    static Int32 AddK(Int32 a) { return a + 5; }

    static Int32 Sum3(Int32 a, Int32 b, Int32 c) { return a + b + c; }

    static Boolean Gt(Int32 a, Int32 b) { return a > b; }

    static Boolean Lt(Int32 a, Int32 b) { return a < b; }

    static Boolean Eq(Int32 a, Int32 b) { return a == b; }

    static Boolean Ne(Int32 a, Int32 b) { return a != b; }

    static Boolean Ge(Double a, Double b) { return a >= b; }

    static Int64 Wide(Int64 a, Int32 b) { return a * b; }

    static Int64 WideAdd(Int64 a, Int64 b) { return a + b; }

    static Double Half(Double a) { return a / 4; }

    static Int32 Fib(Int32 n)
    {
        if (n < 2)
            return n;

        return Fib(n - 1) + Fib(n - 2);
    }

    static Int32 Fact(Int32 n) { return n <= 1 ? 1 : n * Fact(n - 1); }

    static Int32 SumTo(Int32 n) { return n == 0 ? 0 : n + SumTo(n - 1); }

    static String Name(Int32 k) { return k == 0 ? "a" : "b"; }

    static Point Make(Int32 x, Int32 y) { return new Point(x, y); }

    static Int32 Area(Point p) { return p.X * p.Y; }

    static Int32 Counter = 0;

    static void Bump() { Counter += 3; }

    static void Set(Int32 v) { Counter = v; }

    static Int32 Safe(Int32 a, Int32 b)
    {
        try
        {
            return a / b;
        }
        catch (DivideByZeroException)
        {
            return -1;
        }
    }

    static Int32 Tier(Int32 i) { return i * 3; }

    static Boolean Even(Int32 i) { return (i & 1) == 0; }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("values " + ArgI(5) + " " + ConstI() + " " + LocalI(5) + " " + ArgL(123456789012L) + " " + ArgD(2.5) + " " + ArgF(1.5f)
                          + " " + ArgC('x') + " " + (ArgB(true) ? "True" : "False"));

        Console.WriteLine("ops " + Add(5, 7) + " " + Sub(5, 7) + " " + Mul(5, 7) + " " + Div(17, 5) + " " + Mod(17, 8) + " " + And(5, 3) + " "
                          + Shl(1, 3) + " " + AddK(0));

        Console.WriteLine("compare " + (Gt(5, 3) ? "True" : "False") + " " + (Lt(5, 3) ? "True" : "False") + " " + (Eq(4, 4) ? "True" : "False") + " "
                          + (Ne(4, 4) ? "True" : "False") + " " + (Ge(2.5, 2.5) ? "True" : "False"));

        Console.WriteLine("wide " + Wide(1000000000L, 5) + " " + WideAdd(40000000000L, 50000000000L) + " " + Half(11.0));

        Console.WriteLine("recursion " + Fib(30) + " " + Fact(10) + " " + SumTo(10));

        Console.WriteLine("object " + Name(0) + " " + Name(1));

        Point pt = Make(3, 4);
        Console.WriteLine("struct " + pt.X + " " + pt.Y + " " + Area(Make(5, 5)));

        Counter = 0;
        Bump();
        Int32 before = Counter;
        Set(99);
        Console.WriteLine("void " + before + " " + Counter);

        Console.WriteLine("args " + Sum3(1, 2, 4) + " " + Sub(Add(1, 2), 4) + " " + Add(Mul(2, 3), Sub(0, 2)) + " " + ArgD(2.5));

        Box box = new Box(150);
        Console.WriteLine("mixed " + (box.Get() + box.Twice() - 150) + " " + (box.Self().Get() - 121));

        Int32 total = 0, evens = 0;
        for (Int32 i = 0; i < 1000; i++)
        {
            total += Tier(i) - Tier(i - 1) + Safe(i, 1) - i;
            if (Even(i))
                evens++;
        }

        Console.WriteLine("tier " + (evens * 3) + " " + (Safe(100, 0) + 499600 - 99) + " " + (Safe(1, 0) == -1 ? "true" : "false"));
    }
}
