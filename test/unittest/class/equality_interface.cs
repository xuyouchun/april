// IEquality: a type that has Equals(Object) and GetHashCode() together is equal by them, Object.Equals
// and Object.HashOf use it, and the default equality of a type (a Dictionary key) too. A type that
// does not have it is equal by identity. The hash code of null is 0, two nulls are equal.

/* EXPECT

point True False True
plain True False
null True False False 0
boxed True False False True
dictionary 2 b
identity 2
done

*/

import System;

using System;
using System.Collections;

class Point : IEquality
{
    public Int32 X;
    public Int32 Y;

    public Point(Int32 x, Int32 y)
    {
        this.X = x;
        this.Y = y;
    }

    public Boolean Equals(Object? obj)
    {
        Point point = obj as Point;
        return point != null && point.X == this.X && point.Y == this.Y;
    }

    public Int32 GetHashCode() => this.X * 31 + this.Y;
}

class Plain { }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Point a = new Point(1, 2);
        Point b = new Point(1, 2);
        Point c = new Point(2, 1);
        Console.WriteLine("point " + Object.Equals(a, b) + " " + Object.Equals(a, c)
                                  + " " + (Object.HashOf(a) != Object.HashOf(c)));

        Plain p = new Plain();
        Console.WriteLine("plain " + Object.Equals(p, p) + " " + Object.Equals(p, new Plain()));

        Console.WriteLine("null " + Object.Equals(null, null) + " " + Object.Equals(a, null)
                                 + " " + Object.Equals(null, a) + " " + Object.HashOf(null));

        Object boxed = 4;
        Object text = "ab";
        Console.WriteLine("boxed " + Object.Equals(boxed, 4) + " " + Object.Equals(boxed, 5)
                                  + " " + Object.Equals(text, "abc") + " " + Object.Equals(text, "ab"));

        Dictionary<Point, String> map = new Dictionary<Point, String>();
        map.Add(new Point(1, 1), "a");
        map.Add(new Point(2, 2), "b");
        map[new Point(1, 1)] = "a";
        Console.WriteLine("dictionary " + map.Count + " " + map[new Point(2, 2)]);

        Dictionary<Plain, Int32> identity = new Dictionary<Plain, Int32>();
        identity.Add(new Plain(), 1);
        identity.Add(new Plain(), 2);
        Console.WriteLine("identity " + identity.Count);

        Console.WriteLine("done");
    }
}
