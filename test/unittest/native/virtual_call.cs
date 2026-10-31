// Virtual methods, interfaces and casts.

/* EXPECT

2504
2103
100
50
42
29
24
1
True

False

True

False

25
True

4

*/

import System;

using System;

interface IShape
{
    int Area();
    int Sides();
}

abstract class Shape : IShape
{
    public abstract int Area();

    public virtual int Sides()
    {
        return 0;
    }

    public int Twice()
    {
        return Area() * 2;
    }
}

class Square : Shape
{
    public Square(int side)
    {
        __side = side;
    }

    private int __side;

    public override int Area()
    {
        return __side * __side;
    }

    public override int Sides()
    {
        return 4;
    }
}

class Triangle : Shape
{
    public Triangle(int base_, int height)
    {
        __base = base_;
        __height = height;
    }

    private int __base;
    private int __height;

    public override int Area()
    {
        return __base * __height / 2;
    }

    public override int Sides()
    {
        return 3;
    }
}

class Blob : Shape
{
    public override int Area()
    {
        return 1;
    }
}

class Other
{
}

class Program
{
    static int Describe(Shape shape)
    {
        return shape.Area() * 100 + shape.Sides();
    }

    static int ThroughInterface(IShape shape)
    {
        return shape.Area() + shape.Sides();
    }

    [EntryPoint]
    public static void Main()
    {
        Shape a = new Square(5);
        Shape b = new Triangle(6, 7);
        Shape c = new Blob();

        // Virtual calls.
        Console.WriteLine(Describe(a));
        Console.WriteLine(Describe(b));
        Console.WriteLine(Describe(c));

        // A non virtual method that calls a virtual one.
        Console.WriteLine(a.Twice());
        Console.WriteLine(b.Twice());

        // Interface calls.
        Console.WriteLine(ThroughInterface(a));
        Console.WriteLine(ThroughInterface(b));
        Console.WriteLine(ThroughInterface(c));

        // Casts.
        object o = a;
        Console.WriteLine(o is Square);
        Console.WriteLine(o is Triangle);
        Console.WriteLine(o is IShape);
        Console.WriteLine(new Other() is IShape);

        Square s = o as Square;
        Console.WriteLine(s.Area());

        Triangle t = o as Triangle;
        Console.WriteLine(t == null);

        Shape back = (Shape)o;
        Console.WriteLine(back.Sides());
    }
}
