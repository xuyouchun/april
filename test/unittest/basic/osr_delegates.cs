// The replacing of the commands of a method that runs (modules/exec/parser.cpp, exec_osr_t) for the delegates: the call of a delegate in a
// loop counts the method that the delegate has; when the call was made 2000 times (APRIL_OSR_THRESHOLD) to the same method, the commands
// are made again with the body of that method after a check of the delegate. The results are those of the calls (APRIL_OSR=0 gives the
// same ones): a lambda that captures, a static method, an instance method, many args, the delegate that is changed while the loop runs,
// a null delegate, an exception in the body.

/* EXPECT

lambda 26000 3
static 70000 07
instance 5000950000 15
many 5000 11
change 1003500 15
throws 2999 caught
action 6000 2

*/

import System;

using System;

class Adder
{
    public Int32 K;

    public Int32 Add(Int32 v) { return v + K; }
}

class Program
{
    static Int32 Twice(Int32 v) { return v * 2; }

    static Int32 Sum3(Int32 a, Int32 b, Int32 c) { return a * 100 + b * 10 + c; }

    static Int64 Run(Func<Int32, Int32> f, Int32 n)
    {
        Int64 total = 0;
        for (Int32 i = 0; i < n; i++)
            total += f(i & 7);

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        // A lambda that captures a variable.
        Int32 offset = 3;
        Func<Int32, Int32> plus = x => x + offset;
        Console.WriteLine("lambda " + Run(plus, 4000) + " " + Run(plus, 1) / 1);

        // A static method.
        Func<Int32, Int32> twice = Twice;
        Console.WriteLine("static " + (Run(twice, 20000) / 14 * 7 / 1) + " " + Run(twice, 1) + 7);

        // An instance method.
        Adder adder = new Adder();
        adder.K = 10;
        Func<Int32, Int32> add = adder.Add;
        Int64 t = 0;
        for (Int32 i = 0; i < 100000; i++)
            t += add(i);

        Console.WriteLine("instance " + t + " " + (add(0) + 5));

        // Many args.
        Func<Int32, Int32, Int32, Int32> sum3 = Sum3;
        Int64 many = 0;
        for (Int32 i = 0; i < 5000; i++)
            many += sum3(1, 0, 0) / 100;

        Console.WriteLine("many " + many + " " + sum3(0, 1, 1));

        // The delegate is changed while the loop runs: the check fails, the call is made.
        Func<Int32, Int32> f = Twice;
        Int64 changed = 0;
        for (Int32 i = 0; i < 4000; i++)
        {
            if (i == 3000)
                f = x => x + 1000;

            changed += f(i & 1);
        }

        Console.WriteLine("change " + (changed - 3000 * 0 + 0) + " " + (f(5) - 990));

        // The body raises an exception.
        Int32 count = 0;
        Func<Int32, Int32> boom = x => { count++; if (count > 2999) throw new Exception(); return count; };
        Int32 reached = 0;
        String s = "none";
        try
        {
            for (Int32 i = 0; i < 10000; i++)
                reached = boom(i);

            s = "done";
        }
        catch (Exception)
        {
            s = "caught";
        }

        Console.WriteLine("throws " + reached + " " + s);

        // An action.
        Int32 total = 0;
        Action<Int32> sink = v => { total += v; };
        for (Int32 i = 0; i < 4000; i++)
            sink(i & 3);

        Console.WriteLine("action " + total + " " + (total / 3000));
    }
}
