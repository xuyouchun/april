// The replacing of the commands of a method that runs (modules/exec/parser.cpp, exec_osr_t): the virtual and interface calls of a loop
// count what they go to; when a call has been made 2000 times (APRIL_OSR_THRESHOLD) to the same method, the commands are made again, with
// the body of that method after a check of the object, and the loop goes on in them. The results are those of the calls
// (APRIL_OSR=0 gives the same ones): an object of the same class, another class (the check fails: the call is made), a null object,
// an exception in the body, a try block in the method, a method that is called again, a frame that ran the old commands.

/* EXPECT

iface 100000 50
virtual 160000 4
switch 130000 25000
null caught 3000 caught
throws 2999 caught 12
blocks 1500 done
twice 41000 41000
value 3000 1000
many 1400 0

*/

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

class Boom : IShape
{
    public Int32 Limit;
    public Int32 Count;

    public Int32 Area()
    {
        Count++;
        if (Count > Limit)
            throw new Exception();

        return Count;
    }
}

struct Cell : IShape
{
    public Int32 V;

    public Int32 Area() { return V + 1; }
}

abstract class Animal
{
    public abstract Int32 Legs();
}

class Dog : Animal { public override Int32 Legs() { return 4; } }

class Bird : Animal { public override Int32 Legs() { return 2; } }

class Counter
{
    public Int32 Total;

    public virtual Int32 Step(Int32 i) { Total += i; return Total & 7; }
}

class Program
{
    static Int64 Sum(IShape shape, Int32 n)
    {
        Int64 total = 0;
        for (Int32 i = 0; i < n; i++)
            total += shape.Area();

        return total;
    }

    static Int32 Legs(Animal a, Int32 n)
    {
        Int32 total = 0;
        for (Int32 i = 0; i < n; i++)
            total += a.Legs();

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        // One class at the call, many times.
        Square sq = new Square();
        sq.Side = 5;
        Int64 s1 = Sum(sq, 4000000) / 1000;
        Console.WriteLine("iface " + s1 + " " + Sum(sq, 2) / 1);

        Animal dog = new Dog();
        Console.WriteLine("virtual " + Legs(dog, 40000) + " " + Legs(dog, 1));

        // The class changes while the loop runs: the check fails, the call is made.
        IShape[] shapes = new IShape[2];
        Square a = new Square();
        a.Side = 10;
        Rect b = new Rect();
        b.W = 5;
        b.H = 5;
        shapes[0] = a;
        shapes[1] = b;

        Int64 mixed = 0, tail = 0;
        for (Int32 i = 0; i < 10000; i++)
            mixed += shapes[i < 6000? 0 : 1].Area() - (i < 6000? 100 : 0);

        for (Int32 i = 0; i < 1000; i++)
            tail += shapes[1].Area();

        Console.WriteLine("switch " + (mixed + 30000) + " " + (tail));

        // The object becomes null while the loop runs.
        IShape current = sq;
        Int64 got = 0;
        String r = "none";
        try
        {
            for (Int32 i = 0; i < 6000; i++)
            {
                if (i == 3000)
                    current = null;

                got += current.Area() / 25;
            }

            r = "done";
        }
        catch (NullReferenceException)
        {
            r = "caught";
        }

        Console.WriteLine("null " + r + " " + got + " " + (r == "caught" ? "caught" : "x"));

        // The body raises an exception after the commands were made again.
        Boom boom = new Boom();
        boom.Limit = 2999;
        IShape bi = boom;
        Int32 reached = 0;
        String t = "none";
        try
        {
            for (Int32 i = 0; i < 10000; i++)
                reached = bi.Area();

            t = "done";
        }
        catch (Exception)
        {
            t = "caught";
        }

        Console.WriteLine("throws " + reached + " " + t + " " + (Legs(dog, 3)));

        // A try block with a loop: the blocks of the second version find the exception of the first one, and the other way.
        Int32 blocks = 0;
        try
        {
            for (Int32 i = 0; i < 6000; i++)
            {
                blocks += shapes[0].Area() == 100 ? 1 : 0;
                if (i == 1500)
                    blocks -= 0;
            }

            blocks = blocks / 4;
        }
        finally
        {
            blocks += 0;
        }

        Console.WriteLine("blocks " + blocks + " done");

        // A method that is called again: the next call goes on in the new commands from the begin.
        Int64 twice = Sum(sq, 1640) + Sum(sq, 0) + Sum(sq, 0);
        Int64 again = Sum(sq, 1640);
        Console.WriteLine("twice " + twice + " " + again);

        // A struct: the call that has a value for the object is not replaced.
        IShape cell = new Cell();
        Int64 values = 0;
        for (Int32 i = 0; i < 3000; i++)
            values += cell.Area();

        Cell c2 = new Cell();
        c2.V = 3;
        IShape boxed = c2;
        Int64 other = 0;
        for (Int32 i = 0; i < 250; i++)
            other += boxed.Area();

        Console.WriteLine("value " + values + " " + other);

        // Many calls in the loop of a method.
        Counter counter = new Counter();
        Int32 many = 0;
        for (Int32 i = 0; i < 4000; i++)
            many += counter.Step(i) + counter.Step(1) - counter.Step(1);

        Console.WriteLine("many " + (many / 10) + " " + (counter.Total % 1000));
    }
}
