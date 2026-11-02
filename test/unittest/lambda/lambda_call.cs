// A lambda in brackets is called at once: its params with no types take the types of the arguments, its return
// type is the type of the body. A method that returns a delegate is called with the arguments of the delegate.

/* EXPECT

13
23
hi
4
3
50
110
7
void
7

*/

import System;

using System;

class Calc
{
    public static Delegate<Int32, Int32> Adder(Int32 n)
    {
        return (Int32 x) => x + n;
    }

    public Int32 Base = 100;

    public Int32 Run(Int32 v)
    {
        return ((Int32 x) => x * 2)(v) + Base;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int a = 3;
        int x = 10;

        // Implicit params take the types of the arguments.
        Console.WriteLine((y => y + a)(x));
        Console.WriteLine(((p, q) => p * q + a)(4, 5));

        // No params, and a string.
        Console.WriteLine((() => "hi")());
        Console.WriteLine((() => a + 1)());

        // A call of a method that returns a delegate.
        Console.WriteLine(Calc.Adder(1)(2));

        // The result is used.
        int r = ((Int32 y) => y * y)(7) + 1;
        Console.WriteLine(r);

        var c = new Calc();
        Console.WriteLine(c.Run(5));

        // A block that returns nothing.
        ((Int32 v) => { Console.WriteLine(v + a); })(4);
        (() => Console.WriteLine("void"))();

        // Nested.
        Console.WriteLine(((Int32 y) => ((Int32 z) => z + y + a)(y))(2));
    }
}
