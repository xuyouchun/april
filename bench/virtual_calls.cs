// Benchmark: virtual and interface calls.

import System;

using System;

interface IShape
{
    Int32 Area();
}

class Square : IShape
{
    public Int32 Side;
    public Int32 Area() { return Side * Side; }
}

class Rect : IShape
{
    public Int32 W;
    public Int32 H;
    public Int32 Area() { return W * H; }
}

abstract class Animal
{
    public abstract Int32 Legs();
}

class Dog : Animal { public override Int32 Legs() { return 4; } }

class Bird : Animal { public override Int32 Legs() { return 2; } }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        IShape[] shapes = new IShape[2];
        Square square = new Square();
        square.Side = 3;
        Rect rect = new Rect();
        rect.W = 2;
        rect.H = 5;
        shapes[0] = square;
        shapes[1] = rect;

        Animal[] animals = new Animal[2];
        animals[0] = new Dog();
        animals[1] = new Bird();

        Int64 total = 0;
        for (Int32 i = 0; i < 20000000; i++)
        {
            total += shapes[i & 1].Area();
            total += animals[(i >> 1) & 1].Legs();
        }

        Console.WriteLine(total);
    }
}
