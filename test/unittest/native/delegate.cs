// Delegates to static and instance methods, with struct arguments and results.

/* EXPECT

42
10
11
12
15
11
2

*/

import System;

using System;

struct Point
{
    public int X;
    public int Y;

    public Point(int x, int y)
    {
        X = x;
        Y = y;
    }
}

class Counter
{
    public Counter(int start)
    {
        __value = start;
    }

    private int __value;

    public int Next()
    {
        __value = __value + 1;
        return __value;
    }

    public int Add(int a, int b)
    {
        return __value + a + b;
    }
}

class Program
{
    static int Twice(int a)
    {
        return a * 2;
    }

    static Point Move(Point p, int dx)
    {
        return new Point(p.X + dx, p.Y);
    }

    static int Apply(Delegate<int, int> f, int value)
    {
        return f(value);
    }

    [EntryPoint]
    public static void Main()
    {
        // A static method.
        {
            var f = Twice;
            Console.WriteLine(f(21));
            Console.WriteLine(Apply(f, 5));
        }

        // Instance methods keep their instance.
        {
            Counter c = new Counter(10);
            var next = c.Next;
            var add = c.Add;

            Console.WriteLine(next());
            Console.WriteLine(next());
            Console.WriteLine(add(1, 2));
        }

        // Struct arguments and results.
        {
            var move = Move;
            Point p = move(new Point(1, 2), 10);
            Console.WriteLine(p.X);
            Console.WriteLine(p.Y);
        }
    }
}
