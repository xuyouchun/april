// A lambda calls a delegate that it captures: f(1) of a variable of the method that has the lambda.

/* EXPECT

1 2
2
f
nested 4
caught
done

*/

import System;

using System;

class Program
{
    static void Run(Action action) { action(); }

    static Int32 Twice(Int32 x) { return x * 2; }

    [EntryPoint]
    public static void Main()
    {
        Func<Int32, Int32> inc = x => x + 1;

        Run(() => { Console.WriteLine(inc(0) + " " + inc(1)); });

        Func<Int32, Int32> twice = Twice;
        Run(() => { Int32 y = twice(1); Console.WriteLine(y); });

        Action hello = () => { Console.WriteLine("f"); };
        Run(() => { hello(); });

        Run(() => { Run(() => { Console.WriteLine("nested " + twice(inc(1))); }); });

        Func<Int32, Int32> boom = x => throw new InvalidOperationException("x");
        Run(() => { try { boom(1); } catch (Exception e) { Console.WriteLine("caught"); } });

        Console.WriteLine("done");
    }
}
