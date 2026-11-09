// The constraint unmanaged: the numbers, bool, char, enums, and the structs that have only them (nested, generic too).

/* EXPECT

5 2.5 True x True
1,4 True
7
8

*/

import System;

using System;

enum Color { Red, Green }

struct Point
{
    public Int32 X;
    public Int32 Y;
}

struct Line
{
    public Point A;
    public Point B;
    public Color C;
}

struct Pair<T>
{
    public T First;
    public T Second;
}

class Holder<T> where T : unmanaged
{
    public T Value;
}

class Program
{
    static T Same<T>(T value) where T : unmanaged => value;

    // A param that has the constraint is unmanaged for the constraint of another method.
    static T Twice<T>(T value) where T : unmanaged => Same<T>(value);

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Same(5) + " " + Same(2.5) + " " + Same(true) + " " + Same('x') + " " + (Same(Color.Green) == Color.Green));

        Point a = new Point();
        a.X = 1;
        a.Y = 2;

        Point b = new Point();
        b.X = 3;
        b.Y = 4;

        Line l = new Line();
        l.A = a;
        l.B = b;
        l.C = Color.Green;

        Line m = Same(l);
        Console.WriteLine(m.A.X + "," + m.B.Y + " " + (m.C == Color.Green));

        Pair<Int32> pair = new Pair<Int32>();
        pair.First = 3;
        pair.Second = 4;

        Pair<Int32> back = Same(pair);
        Console.WriteLine(back.First + back.Second);

        Holder<Point> holder = new Holder<Point>();
        holder.Value = a;
        Console.WriteLine(holder.Value.Y + Twice(6));
    }
}
