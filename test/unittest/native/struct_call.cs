// Structs: constructors, methods, arguments and results.

/* EXPECT

7
13
24
25
1000
3
7
8
8
9
11
1
40
30
99

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

    public int Sum()
    {
        return X + Y;
    }

    public void Move(int dx, int dy)
    {
        X = X + dx;
        Y = Y + dy;
    }
}

struct Line
{
    public Point From;
    public Point To;
}

class Program
{
    static int Length2(Point p)
    {
        return p.X * p.X + p.Y * p.Y;
    }

    static void Change(Point p)
    {
        p.X = 1000;
        Console.WriteLine(p.X);
    }

    static Point Make(int x, int y)
    {
        Point p = new Point(x, y);
        return p;
    }

    static Point Add(Point a, Point b)
    {
        return new Point(a.X + b.X, a.Y + b.Y);
    }

    [EntryPoint]
    public static void Main()
    {
        // Constructor and methods.
        {
            Point p = new Point(3, 4);
            Console.WriteLine(p.Sum());

            p.Move(10, 20);
            Console.WriteLine(p.X);
            Console.WriteLine(p.Y);
        }

        // Passed by value.
        {
            Point p = new Point(3, 4);
            Console.WriteLine(Length2(p));

            Change(p);
            Console.WriteLine(p.X);
        }

        // Returned.
        {
            Point p = Make(7, 8);
            Console.WriteLine(p.X);
            Console.WriteLine(p.Y);

            Point q = Add(p, new Point(1, 1));
            Console.WriteLine(q.X);
            Console.WriteLine(q.Y);
            Console.WriteLine(Make(5, 6).Sum());
        }

        // Nested.
        {
            Line line;
            line.From = new Point(1, 2);
            line.To = new Point(30, 40);

            Console.WriteLine(line.From.X);
            Console.WriteLine(line.To.Y);

            Line copy;
            copy = line;
            copy.To.X = 99;
            Console.WriteLine(line.To.X);
            Console.WriteLine(copy.To.X);
        }
    }
}
