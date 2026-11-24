// A case can have a condition (case 1 when a > b:) and declare a variable of a type (case String s:), the variable is
// the one of the case. The labels are tested in the order that they are written, the first one that matches is the case.

/* EXPECT

int 5
big int 50
string hi 2
long 7
null
other
guard a
guard b
guard none
const 1 positive
const 1 other
shapes circle 3 square 4
same name text 4
fall 1 2
loop 1 2 1 3
when 3

*/

import System;

using System;

class Shape { }

class Circle : Shape
{
    public Int32 Radius;
    public Circle(Int32 r) { Radius = r; }
}

class Square : Shape
{
    public Int32 Side;
    public Square(Int32 s) { Side = s; }
}

class Program
{
    static String Describe(Object o)
    {
        switch (o)
        {
            case Int32 n when n > 10:
                return "big int " + n;

            case Int32 n:
                return "int " + n;

            case String s when s.Length > 0:
                return "string " + s + " " + s.Length;

            case Int64 l:
                return "long " + l;

            case null:
                return "null";

            default:
                return "other";
        }
    }

    static String Guard(Int32 k, Boolean a, Boolean b)
    {
        switch (k)
        {
            case 1 when a:
                return "guard a";

            case 1 when b:
                return "guard b";

            default:
                return "guard none";
        }
    }

    static String Const(Int32 k, Int32 limit)
    {
        switch (k)
        {
            case 1 when limit > 0:
                return "const 1 positive";

            case 1:
                return "const 1 other";

            default:
                return "const ?";
        }
    }

    static String Shapes(Shape shape)
    {
        switch (shape)
        {
            case Circle c:
                return "circle " + c.Radius;

            case Square s:
                return "square " + s.Side;

            default:
                return "shape";
        }
    }

    // The variables of two cases have the same name.
    static String SameName(Object o)
    {
        switch (o)
        {
            case Int32 v:
                return "int " + v;

            case String v:
                return "same name " + v + " " + v.Length;

            default:
                return "?";
        }
    }

    static String Fall(Object o)
    {
        String r = "";

        switch (o)
        {
            case Int32 n when n == 1:
                r += "1 ";

            case String s:
                r += "2";
                break;

            default:
                r += "default";
                break;
        }

        return r;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Describe(5));
        Console.WriteLine(Describe(50));
        Console.WriteLine(Describe("hi"));
        Console.WriteLine(Describe((Int64)7));
        Console.WriteLine(Describe(null));
        Console.WriteLine(Describe(""));

        Console.WriteLine(Guard(1, true, false));
        Console.WriteLine(Guard(1, false, true));
        Console.WriteLine(Guard(1, false, false));
        Console.WriteLine(Const(1, 5));
        Console.WriteLine(Const(1, 0));

        Console.WriteLine("shapes " + Shapes(new Circle(3)) + " " + Shapes(new Square(4)));
        Console.WriteLine(SameName("text"));
        Console.WriteLine("fall " + Fall(1));

        // In a loop: break leaves the switch, the variable is made again.
        String line = "loop";
        Object[] items = new Object[] { 1, 2, "x", 3 };
        foreach (Object item in items)
        {
            switch (item)
            {
                case Int32 n when n > 0:
                    line += " " + n;
                    break;

                case String s:
                    line += " " + s.Length;
                    break;
            }
        }

        Console.WriteLine(line);

        // when is a keyword only after a label.
        Int32 when = 3;
        Console.WriteLine("when " + when);
    }
}
