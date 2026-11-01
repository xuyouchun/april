// The type of a lambda that is not converted to a delegate: Delegate<TReturn, TArgs...>, TReturn is the type of the body, or of the
// values that a block returns, and Void for a block that returns no value. Blocks are called at once too.

/* EXPECT

6
13
19
big small
8
9
9
9
42
10
4
7
42

*/

import System;

using System;

class Program
{
    static int Apply(Delegate<Int32, Int32> f, Int32 v)
    {
        return f(v);
    }

    [EntryPoint]
    public static void Main()
    {
        int a = 3;

        // The type of a lambda that is not converted to a delegate is Delegate<TReturn, TArgs...>.
        var f = () => a * 2;
        Console.WriteLine(f());

        var g = (Int32 x) => { return x + a; };
        Console.WriteLine(g(10));

        // A block that has locals, and returns a value.
        var h = (Int32 x) =>
        {
            int t = x * x;
            int u = t + a;
            return u;
        };
        Console.WriteLine(h(4));

        // More than one return.
        var k = (Int32 x) =>
        {
            if (x > 5)
            {
                return "big";
            }

            return "small";
        };
        Console.WriteLine(k(9) + " " + k(1));

        // Void: a block with no value to return.
        var p = (Int32 x) => { Console.WriteLine(x + a); };
        p(5);

        // A lambda that is called at once.
        Console.WriteLine((() => { return a * 3; })());
        Console.WriteLine(((Int32 x) => { int y = x + 1; return y * a; })(2));
        Console.WriteLine((x => { int y = x + 1; return y * a; })(2));

        // A lambda that returns a lambda.
        var curry = (Int32 x) => { return (Int32 y) => x * 10 + y; };
        var c = curry(4);
        Console.WriteLine(c(2));

        // The delegate types of the lambdas are the same as written.
        Delegate<Int32> same = f;
        Delegate<Int32, Int32> same2 = g;
        Delegate<Void, Int32> same3 = p;
        Console.WriteLine(same() + same2(1));
        same3(1);

        // Converted to a delegate: its return type is that of the delegate.
        Delegate<Int64, Int32> wide = (Int32 x) => { return x; };
        Console.WriteLine(wide(7));
        Console.WriteLine(Apply(x => { int y = x * 2; return y; }, 21));
    }
}
