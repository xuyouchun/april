// new(...) is new Type(...) of the type that the context wants: a variable, an assignment, a field, a return, an argument.

/* EXPECT

list 2
dict 1
point 3 4
assign 5
field 7 0
return 9
arg 11
array 2
nested 1 2

*/

import System;

using System;
using System.Collections;

class Point
{
    public Int32 X;
    public Int32 Y;

    public Point(Int32 x, Int32 y) { X = x; Y = y; }
}

class Holder
{
    public Point Item = new(7, 0);
    public List<Int32> Items = new();
}

class Program
{
    static Point Make() { return new(9, 0); }

    static Int32 Sum(Point p) { return p.X + p.Y; }

    [EntryPoint]
    public static void Main()
    {
        List<Int32> list = new();
        list.Add(1);
        list.Add(2);
        Console.WriteLine("list " + list.Count);

        Dictionary<String, Int32> dict = new();
        dict["a"] = 1;
        Console.WriteLine("dict " + dict.Count);

        Point point = new(3, 4);
        Console.WriteLine("point " + point.X + " " + point.Y);

        Point other;
        other = new(5, 0);
        Console.WriteLine("assign " + other.X);

        Holder holder = new();
        Console.WriteLine("field " + holder.Item.X + " " + holder.Items.Count);

        Console.WriteLine("return " + Make().X);
        Console.WriteLine("arg " + Sum(new(11, 0)));

        Point[] points = new Point[] { new(1, 0), new(2, 0) };
        Console.WriteLine("array " + points[1].X);

        List<Point> nested = new() { new(1, 2) };
        Console.WriteLine("nested " + nested[0].X + " " + nested[0].Y);
    }
}
