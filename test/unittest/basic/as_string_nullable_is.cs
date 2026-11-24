// o as String gives the String that the object holds (null when it is not one), n is T of a nullable is true when it has a
// value, n is int v takes it.

/* EXPECT

as hello 5
as null True
as new hello!
is 3
is no value
is True False
is not 7 ok
ctor 3 5

*/

import System;
using System;
using System.Collections;

class Maker<X> where X : new()
{
    public static Int32 Size() { return 3; }
}

class Two
{
    public Int32 A;

    public Two() { A = 3; }

    public Two(Int32 a) { A = a; }
}

class Program
{
    static String Describe(Int32? n)
    {
        if (n is Int32 v)
            return "is " + v;

        return "is no value";
    }

    [EntryPoint]
    public static void Main()
    {
        Object o = "hello";
        String s = o as String;
        Console.WriteLine("as " + s + " " + s.Length);

        Object p = 1;
        String t = p as String;
        Console.WriteLine("as null " + (t == null));

        Object q = null;
        String u = q as String;
        Console.WriteLine("as new " + ((o as String) + "!"));

        Console.WriteLine(Describe(3));
        Console.WriteLine(Describe(null));

        Int32? a = 4, b = null;
        Console.WriteLine("is " + (a is Int32) + " " + (b is Int32));

        Int32? c = 7;
        if (!(c is Int32 w))
            Console.WriteLine("is not");
        else
            Console.WriteLine("is not " + w + " ok");

        // A type that has many constructors satisfies new().
        Console.WriteLine("ctor " + Maker<Two>.Size() + " " + new Two(5).A);
    }
}
