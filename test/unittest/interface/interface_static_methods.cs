// A static method of an interface has a body, and is called like the static method of a class;
// it is not virtual: it has no slot in the virtual table, the methods around it are called as
// usual.

/* EXPECT

3
20
4
12

*/

import System;

using System;

interface IShape
{
    // Before the methods that are virtual.
    public static int Make() { return 3; }

    int Area();

    static int Twice(int x) { return x * 2; }

    int Sides();
}

class Square : IShape
{
    public int Area() { return 20; }
    public int Sides() { return 4; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(IShape.Make());

        IShape shape = new Square();
        Console.WriteLine(shape.Area());
        Console.WriteLine(shape.Sides());
        Console.WriteLine(IShape.Twice(6));
    }
}
