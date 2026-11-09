// The compound assignments (+=, -=, *=, ...) and ++, -- of properties and indexers: the getter is called, the value is
// computed, the setter is called with it.

/* EXPECT

15
14
16
16 32 31
7
postfix 5 6 8 6

*/

import System;

using System;

class Box
{
    Int32 __v;

    public Int32 V { get => __v; set { __v = value; } }

    public Int32 this[Int32 i] { get => __v + i; set { __v = value - i; } }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box b = new Box();

        b.V = 10;
        b.V += 5;
        Console.WriteLine(b.V);

        b[1] = 20;
        b[1] -= 6;
        Console.WriteLine(b[1]);

        b[2]++;
        Console.WriteLine(b[2]);

        b.V = 15;
        b.V++;
        Int32 first = b.V;
        b.V *= 2;
        Int32 second = b.V;
        b.V -= 1;
        Console.WriteLine(first + " " + second + " " + b.V);

        b.V = 8;
        b.V--;
        Console.WriteLine(b.V);

        // The value of x++ is the old value.
        b.V = 5;
        Int32 x = b.V++;
        Int32 y = b.V--;
        Int32 z = b[3]++;
        Console.WriteLine("postfix " + x + " " + y + " " + z + " " + b.V);
    }
}
