// new T() of a struct, or of a number: its default value. (The programs run by the JIT and the native back end
// that make a struct from a generic param are run with the interpreter.)

/* EXPECT

struct 0 0
number 0
default 0

*/

import System;

using System;

struct Pair
{
    public Int32 X;
    public Int32 Y;
}

class Maker<X> where X : new()
{
    public X Make() { return new X(); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Pair pair = new Maker<Pair>().Make();
        Console.WriteLine("struct " + pair.X + " " + pair.Y);

        Int32 number = new Maker<Int32>().Make();
        Console.WriteLine("number " + number);

        Int64 big = new Maker<Int64>().Make();
        Console.WriteLine("default " + big);
    }
}
