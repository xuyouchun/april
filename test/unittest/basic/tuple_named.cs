// The items of a tuple that is written with names have them: tuple.Number. They are the names of the declaration (a
// variable, a field, a param, a return type, a typedef), and not Item1; an item that has no name is ItemN.

/* EXPECT

1 b
x5
8
r
2
5
4l

*/

import System;
using System;

typedef MyTuple = (Int32 Number, String Str);
typedef Pair<T> = (T First, T Second);

class Holder
{
    public (Int32 X, Int32 Y) P = (1, 2);
    public static (Int32 Id, String Name) Make() { return (3, "r"); }
    public static Int32 Sum((Int32 A, Int32 B) t) { return t.A + t.B; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        (Int32 Number, String Str) a = (1, "b");
        Console.WriteLine(a.Number + " " + a.Str);
        MyTuple m = (5, "x");
        Console.WriteLine(m.Str + m.Number);
        Pair<Int32> p = (7, 8);
        Console.WriteLine(p.Second);
        Console.WriteLine(Holder.Make().Name);
        Holder h = new Holder();
        Console.WriteLine(h.P.Y);
        Console.WriteLine(Holder.Sum((2, 3)));
        (Int32, String Label) half = (4, "l");
        Console.WriteLine(half.Item1 + half.Label);
    }
}
