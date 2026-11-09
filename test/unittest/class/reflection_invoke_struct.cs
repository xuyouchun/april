// Method.Invoke with structs: a struct as an argument (it is copied: the box that is passed does not
// change), as a result, and as the object of a method of the struct.

/* EXPECT

add 4 6
static 3 4
length 25
mutate 9 2
unchanged 3
done

*/

import System;

using System;
using System.Reflection;

struct Point
{
    public Int32 X;
    public Int32 Y;

    public Point(Int32 x, Int32 y) { X = x; Y = y; }

    public Point Add(Point other) { return new Point(X + other.X, Y + other.Y); }

    public Int32 Length() { return X * X + Y * Y; }

    public Void Move(Int32 dx) { X = X + dx; }

    public static Point Make(Int32 x, Int32 y) { return new Point(x, y); }

    public static Void Shift(Point p) { p.X = 99; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Point a = new Point(1, 2);
        Point b = new Point(3, 4);
        Type type = Type.GetType(a);

        Object?[] one = new Object?[1];
        one[0] = b;

        Point sum = (Point)type.GetMethod("Add", new Type[] { type }).Invoke(a, one);
        Console.WriteLine("add " + sum.X + " " + sum.Y);

        Object?[] two = new Object?[2];
        two[0] = 3;
        two[1] = 4;
        Point made = (Point)type.GetMethod("Make", new Type[] { Type.GetType(1), Type.GetType(1) }).Invoke(null, two);
        Console.WriteLine("static " + made.X + " " + made.Y);

        Console.WriteLine("length " + type.GetMethod("Length", new Type[0]).Invoke(b, null));

        // The object of a method of a struct is the box: the method changes it.
        Object boxed = a;
        Object?[] move = new Object?[1];
        move[0] = 8;
        type.GetMethod("Move", new Type[] { Type.GetType(1) }).Invoke(boxed, move);
        Point moved = (Point)boxed;
        Console.WriteLine("mutate " + moved.X + " " + moved.Y);

        // A struct that is an argument is a copy.
        Object boxedB = b;
        Object?[] shift = new Object?[1];
        shift[0] = boxedB;
        type.GetMethod("Shift", new Type[] { type }).Invoke(null, shift);
        Point after = (Point)boxedB;
        Console.WriteLine("unchanged " + after.X);

        Console.WriteLine("done");
    }
}
