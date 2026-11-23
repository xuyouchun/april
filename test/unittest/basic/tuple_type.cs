// The type of a tuple is written (Int32, String), or with the names of its items (Int32 Number, String Str): it is
// Tuple<Int32, String>. It is a type name as the others: a variable, a field, a param, a return type, a generic argument,
// a typedef, nullable.

/* EXPECT

1 b
x 5
8
r
True
5
3
two
5 9

*/

import System;

using System;
using System.Collections;

typedef MyTuple = (Int32 Number, String Str);
typedef Pair<T> = (T First, T Second);

class Holder
{
    public (Int32, String) Field = (5, "five");

    public static (Int32, String) Make() { return (3, "r"); }

    public static Int32 Sum((Int32, Int32) t) { return t.Item1 + t.Item2; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        (Int32 Number, String Str) a = (1, "b");
        Console.WriteLine(a.Number + " " + a.Str);

        MyTuple m = (5, "x");
        Console.WriteLine(m.Str + " " + m.Number);

        Pair<Int32> p = (7, 8);
        Console.WriteLine(p.Second);

        Console.WriteLine(Holder.Make().Item2);

        (Int32, String)? n = null;
        Console.WriteLine(n == null);

        Console.WriteLine(Holder.Sum((2, 3)));

        Func<Int32, (Int32, Int32)> f = t => t.Item1 + t.Item2;
        Console.WriteLine(f((1, 2)));

        List<(Int32, String)> list = new List<(Int32, String)>();
        list.Add((1, "one"));
        list.Add((2, "two"));
        Console.WriteLine(list[1].Item2);

        Tuple<Int32, Int32> plain = (5, 9);
        (Int32, Int32) same = plain;
        Console.WriteLine(same.Item1 + " " + same.Item2);
    }
}
