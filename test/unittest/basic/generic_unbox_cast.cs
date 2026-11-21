// (T)obj, T is a generic param: the value of the box (a number, an enum, a string), the object (a class), null, and the cast
// that fails; it is assigned to a variable and to an out parameter too.

/* EXPECT

42 Green text True 2.5 True
Blue Blue 7 7
cast

*/

import System;

using System;

enum Color { Red, Green = 5, Blue }

class G
{
    public static T Cast<T>(Object o) { return (T)o; }

    public static Boolean Direct<T>(Object o, out T r) { r = (T)o; return true; }

    public static Boolean Local<T>(Object o, out T r) { T x = (T)o; r = x; return true; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Object a = 42;
        Console.WriteLine("" + G.Cast<Int32>(a) + " " + G.Cast<Color>(Color.Green) + " " + G.Cast<String>("text") + " "
                          + (G.Cast<Program>(null) == null) + " " + G.Cast<Double>(2.5) + " " + G.Cast<Boolean>(true));

        Color c1, c2;
        Int32 i1, i2;
        G.Direct<Color>(Color.Blue, out c1);
        G.Local<Color>(Color.Blue, out c2);
        G.Direct<Int32>(7, out i1);
        G.Local<Int32>(7, out i2);
        Console.WriteLine("" + c1 + " " + c2 + " " + i1 + " " + i2);

        try { G.Cast<Int64>(a); } catch (InvalidCastException e) { Console.WriteLine("cast"); }
    }
}
