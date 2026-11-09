// IStringable: a type that has a text of its own implements it, Object.ToString(obj)
// returns it (for the objects of any static type, the concatenation too). A type
// that does not have it has the full name of its type as text: the namespace, the host types and
// the generic arguments. The text of null is an empty string.

/* EXPECT

point (1,2)
object (1,2)
concat [(1,2)] [(1,2)]
helper (1,2) [] 7
plain Plain
named Shapes.Circle
nested Shapes.Circle.Edge
generic Collections.List<Int32>
generic2 Pair<Int32,String>
done

*/

import System;

using System;
using System.Collections;

class Point : IStringable
{
    public Int32 X;
    public Int32 Y;

    public Point(Int32 x, Int32 y)
    {
        this.X = x;
        this.Y = y;
    }

    public String ToString() => "(" + this.X + "," + this.Y + ")";
}

class Plain { }

class Pair<A, B> { }

namespace Shapes
{
    class Circle
    {
        public class Edge { }
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Point p = new Point(1, 2);
        Console.WriteLine("point " + p.ToString());

        Object o = p;
        Console.WriteLine("object " + Object.ToString(o));
        Console.WriteLine("concat [" + o + "] [" + p + "]");

        Object nothing = null;
        Object seven = 7;
        Console.WriteLine("helper " + Object.ToString(o) + " [" + Object.ToString(nothing) + "] "
                                    + Object.ToString(seven));

        Object plain = new Plain();
        Console.WriteLine("plain " + Object.ToString(plain));

        Object circle = new Shapes.Circle();
        Object edge = new Shapes.Circle.Edge();
        Console.WriteLine("named " + Object.ToString(circle));
        Console.WriteLine("nested " + Object.ToString(edge));

        Object list = new List<Int32>();
        Console.WriteLine("generic " + Object.ToString(list));

        Object pair = new Pair<Int32, String>();
        Console.WriteLine("generic2 " + Object.ToString(pair));

        Console.WriteLine("done");
    }
}
