// The call of Invoke of a delegate goes to the method of the delegate without the frame of Invoke (the interpreter, commands.cpp,
// __delegate_invoke_direct). The first call of Invoke makes its commands, the next ones are direct: every call is made many times.
// Static methods, instance methods, lambdas that capture, many args, no result, a result that is a struct (it keeps the frame), an
// exception that the method raises, delegates that call delegates. APRIL_INLINE_ACCESSORS=0 gives the same results.

/* EXPECT

static 500 42
instance 5051 5051
lambda 12 10 23900
many 21 600
action 5050
struct 30 297 8
throws caught 100 caught
nested 100 15
change 5 25

*/

import System;

using System;

struct Pair
{
    public Int32 A;
    public Int32 B;

    public Pair(Int32 a, Int32 b) { A = a; B = b; }
}

class Counter
{
    public Int32 Total;

    public Int32 Add(Int32 v) { Total += v; return Total; }

    public void Bump(Int32 v) { Total += v; }
}

class Program
{
    static Int32 Twice(Int32 x) { return x * 2; }

    static Int32 Sum4(Int32 a, Int32 b, Int32 c, Int32 d) { return a + b + c + d; }

    static Pair Make(Int32 a, Int32 b) { return new Pair(a, b); }

    static Int32 Boom(Int32 x)
    {
        if (x < 0)
            throw new Exception();

        return x + 1;
    }

    static Int32 Apply(Func<Int32, Int32> f, Int32 v) { return f(v); }

    [EntryPoint]
    public static void Main()
    {
        Func<Int32, Int32> twice = Twice;
        Int32 s = 0;
        for (Int32 i = 0; i < 500; i++)
            s += twice(i) - twice(i - 1) - 1;

        Console.WriteLine("static " + s + " " + twice(21));

        Counter counter = new Counter();
        Func<Int32, Int32> add = counter.Add;
        Action<Int32> bump = counter.Bump;
        for (Int32 i = 1; i <= 100; i++)
            add(i);

        bump(1);
        Console.WriteLine("instance " + counter.Total + " " + add(0));

        Int32 offset = 10;
        Func<Int32, Int32> plus = x => x + offset;
        Func<Int32, Int32> twicePlus = x => plus(plus(x));
        Int64 total = 0;
        for (Int32 i = 0; i < 200; i++)
            total += twicePlus(i);

        offset = 1;
        Console.WriteLine("lambda " + plus(11) + " " + twicePlus(8) + " " + total);

        Func<Int32, Int32, Int32, Int32, Int32> sum4 = Sum4;
        Int32 many = 0;
        for (Int32 i = 0; i < 100; i++)
            many += sum4(i, 1, 2, 3) - i;

        Console.WriteLine("many " + sum4(5, 6, 7, 3) + " " + many);

        Int32 acc = 0;
        Action<Int32> add_acc = v => { acc += v; };
        for (Int32 i = 1; i <= 100; i++)
            add_acc(i);

        Console.WriteLine("action " + acc);

        // A delegate that returns a struct: its Invoke has the address of the result under the args.
        Func<Pair, Int32, Int32> make = Make;
        Pair last = new Pair();
        for (Int32 i = 0; i < 100; i++)
            last = make(i, i * 2);

        Pair p = make(10, 20);
        Console.WriteLine("struct " + (p.A + p.B) + " " + (last.A + last.B) + " " + make(4, 8).B);

        Func<Int32, Int32> boom = Boom;
        String r1 = "none", r2 = "none";
        Int32 got = 0;
        for (Int32 i = 0; i < 100; i++)
            got = boom(i);

        try { boom(-1); r1 = "made"; }
        catch (Exception) { r1 = "caught"; }

        try { r2 = "made " + boom(-5); }
        catch (Exception) { r2 = "caught"; }

        Console.WriteLine("throws " + r1 + " " + got + " " + r2);

        Int32 nested = 0;
        for (Int32 i = 0; i < 10; i++)
            nested += Apply(x => Apply(y => y * 2, x) + 1, i);

        Console.WriteLine("nested " + nested + " " + (Apply(Twice, 5) + 5));

        // The delegate is changed: the next calls go to the new method.
        Func<Int32, Int32> f = Twice;
        Int32 before = f(2) + 1;
        f = x => x * x;
        Console.WriteLine("change " + before + " " + f(5));
    }
}
