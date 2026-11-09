// Object initializers: new T(args) { Name = value, ... } sets the fields and the properties after the
// constructor; a property that has an init accessor is set only there (and in the constructors).

/* EXPECT

a 3 4
b 1 2 5
ctor 7 5
nested 9
value 8 9
expression 10
init 6 name
result 5

*/

import System;

using System;

class Point
{
    public Int32 X;
    public Int32 Y;
}

class Box
{
    public Int32 A;
    public Int32 B;
    public Int32 Total { get; set; }

    public Box() { this.A = 1; this.B = 2; }

    public Box(Int32 a) { this.A = a; this.B = 100; }
}

class Named
{
    public Int32 Id { get; init; }
    public String Name { get; init; }
    public Int32 Count { get; set; }

    public Named() { this.Id = 5; }

    public Named(Int32 id) { this.Id = id; }
}

class Outer
{
    public Point Inner;
}

class Program
{
    static Int32 Twice(Int32 n) { return n * 2; }

    [EntryPoint]
    public static void Main()
    {
        // The members are set after the constructor.
        Point a = new Point() { X = 3, Y = 4 };
        Console.WriteLine("a " + a.X + " " + a.Y);

        // A property too, and the values of the constructor stay when they are not set.
        Box b = new Box { Total = 5 };
        Console.WriteLine("b " + b.A + " " + b.B + " " + b.Total);

        Box c = new Box(7) { B = 5 };
        Console.WriteLine("ctor " + c.A + " " + c.B);

        // The initializer of an object that is a member.
        Outer o = new Outer { Inner = new Point { X = 9 } };
        Console.WriteLine("nested " + o.Inner.X);

        // The values are expressions, they see the variables.
        Int32 n = 4;
        Point d = new Point { X = n * 2, Y = n + 5 };
        Console.WriteLine("value " + d.X + " " + d.Y);

        // The object is used as the value of an expression, as an argument.
        Console.WriteLine("expression " + (new Point { X = 10 }.X));

        // An init accessor: set in the initializer.
        Named named = new Named() { Id = 6, Name = "name", Count = 1 };
        Console.WriteLine("init " + named.Id + " " + named.Name);

        // And the constructor sets it too.
        Named made = new Named();
        Console.WriteLine("result " + made.Id);
    }
}
