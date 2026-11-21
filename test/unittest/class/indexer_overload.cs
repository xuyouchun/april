// A class can have many indexers, with different parameters (a type, a count): as the methods that they call, they
// are overloads. An interface too.

/* EXPECT

int3 strx two12
8 3
set 5 a=7 b=9

*/

import System;

using System;

class Coll
{
    public String this[Int32 i] { get { return "int" + i; } }
    public String this[String s] { get { return "str" + s; } }
    public String this[Int32 a, Int32 b] { get { return "two" + a + b; } }
}

interface IBoth
{
    Int32 this[Int32 i] { get; }
    Int32 this[String s] { get; }
}

class Impl : IBoth
{
    public Int32 this[Int32 i] { get { return i * 2; } }
    public Int32 this[String s] { get { return s.Length; } }
}

class Cell
{
    public Int32 A;
    public Int32 B;

    public Int32 this[Int32 i]
    {
        get { return A + i; }
        set { A = value; }
    }

    public Int32 this[String name]
    {
        get { return name == "a"? A : B; }
        set
        {
            if (name == "a")
                A = value;
            else
                B = value;
        }
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Coll c = new Coll();
        Console.WriteLine(c[3] + " " + c["x"] + " " + c[1, 2]);

        IBoth b = new Impl();
        Console.WriteLine(b[4] + " " + b["abc"]);

        Cell cell = new Cell();
        cell[0] = 5;
        cell["a"] = 7;
        cell["b"] = 9;
        Console.WriteLine("set " + (cell[0] - 2) + " a=" + cell["a"] + " b=" + cell["b"]);
    }
}
