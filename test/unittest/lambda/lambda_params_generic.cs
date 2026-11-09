// A method with a params generic parameter takes a delegate and the arguments of it:
// R Build<R, TArgs ...>(Delegate<R, TArgs ...> function, TArgs ... args) => function(args ...).
// The types are told by the arguments, the lambda gets the types of its params from them, the
// items are the arguments of the call of the delegate (args ... is many arguments, args is the
// address of them, for the methods of the runtime).

/* EXPECT

delegate 3
lambda 12
three 6
text a7
eight 36
typed 5
class 112
struct 134
none 42
mixed 4:x:True
converted 6
twice 14
forward 48
done

*/

import System;

using System;

static class Binder
{
    public static R Build<R, TArgs ...>(Delegate<R, TArgs ...> function, TArgs ... args)
        => function(args ...);

    // The items go on as the arguments of another method that has them.
    public static Int32 Twice<TArgs ...>(Delegate<Int32, TArgs ...> function, TArgs ... args)
        => Build(function, args ...) + Build(function, args ...);

    public static Int32 Forward<TArgs ...>(Delegate<Int32, Int32, TArgs ...> function, Int32 first,
                                                                               TArgs ... args)
        => Twice(function, first, args ...);
}

class Pair
{
    public Int32 A;
    public Int32 B;

    public Pair(Int32 a, Int32 b)
    {
        A = a;
        B = b;
    }
}

struct Point
{
    public Int32 X;
    public Int32 Y;

    public Point(Int32 x, Int32 y)
    {
        X = x;
        Y = y;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Delegate<Int32, Int32, Int32> add = (x, y) => x + y;

        Console.WriteLine("delegate " + Binder.Build(add, 1, 2));
        Console.WriteLine("lambda " + Binder.Build((x, y) => x * y, 3, 4));
        Console.WriteLine("three " + Binder.Build((x, y, z) => x + y + z, 1, 2, 3));
        Console.WriteLine("text " + Binder.Build((s, n) => s + n, "a", 7));
        Console.WriteLine("eight " + Binder.Build(
            (a1, a2, a3, a4, a5, a6, a7, a8) => a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8,
            1, 2, 3, 4, 5, 6, 7, 8));
        Console.WriteLine("typed " + Binder.Build((Int32 x, Int32 y) => x - y, 9, 4));
        Console.WriteLine("class " + Binder.Build((Pair p, Int32 k) => p.A * 10 + p.B + k,
                                                  new Pair(1, 2), 100));
        Console.WriteLine("struct " + Binder.Build((Point p, Int32 k) => p.X * 10 + p.Y + k,
                                                   new Point(3, 4), 100));
        Console.WriteLine("none " + Binder.Build(() => 42));
        Console.WriteLine("mixed " + Binder.Build((Int32 a, String b, Boolean c) => a + ":" + b + ":" + c,
                                                  4, "x", true));
        // The arguments are converted to the types of the params of the lambda.
        Console.WriteLine("converted " + Binder.Build(
            (Int64 a, Double b, Boolean c) => c? a + 1 : 0, 5, 2.5, true));

        Console.WriteLine("twice " + Binder.Twice((x, y) => x + y, 3, 4));
        Console.WriteLine("forward " + Binder.Forward((x, y, z) => x * y * z, 2, 3, 4));
        Console.WriteLine("done");
    }
}
