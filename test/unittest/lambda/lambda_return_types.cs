// A lambda that is not converted to a delegate returns the type that all its return statements
// are converted to: not the type of the first one.

/* EXPECT

True
str
1
5000000000
True
False
7
animal
True
unsigned 4000000000
signed -5

*/

import System;

using System;

class Animal
{
    public String Name() { return "animal"; }
}

class Dog : Animal { }

class Cat : Animal { }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // null and String: String.
        var a = (int x) => { if (x > 0) { return null; } return "str"; };
        Console.WriteLine(a(1) == null);
        Console.WriteLine(a(0));

        // int and long: long.
        var b = (int x) => { if (x > 0) { return 1; } return 5000000000; };
        Console.WriteLine(b(1));
        Console.WriteLine(b(0));

        // null and int: int?.
        var c = (int x) => { if (x > 0) { return null; } return 7; };
        Console.WriteLine(c(1) == null);
        Console.WriteLine(c(0) == null);
        int? n = c(0);
        Console.WriteLine(n.Value);

        // Two classes: their base class.
        var d = (int x) => { if (x > 0) { return new Dog(); } return new Cat(); };
        Console.WriteLine(d(1).Name());

        // Only null: object.
        var e = (int x) => { return null; };
        Console.WriteLine(e(0) == null);

        // Signed and unsigned: the signed type that holds both.
        uint big = 4000000000;
        int small = -5;
        var f = (int x) => { if (x > 0) { return big; } return small; };
        Console.WriteLine("unsigned " + f(1));
        Console.WriteLine("signed " + f(0));
    }
}
