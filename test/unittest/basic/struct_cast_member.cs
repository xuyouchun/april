// A member of a struct that a cast to its own type gives: ((P)new P(1, 2)).X. The cast changes nothing, the value
// is a temp variable that the member is read from (it crashed, and a method of it got a wrong this).

/* EXPECT

8
c
8
1
3
1
5
2
x

*/

import System;
using System;

struct P
{
    public Int32 X;
    public Int32 Y;

    public P(Int32 x, Int32 y) { X = x; Y = y; }

    public Int32 GetX() { return X; }
    public Int32 Sum() { return X + Y; }
}

class Program
{
    static P Make(Int32 x) { return new P(x, 1); }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(((Tuple<Int32, String>)new Tuple<Int32, String>(8, "c")).Item1);
        Console.WriteLine(((Tuple<Int32, String>)new Tuple<Int32, String>(8, "c")).Item2);
        Console.WriteLine(((Tuple<Int32, String>)(8, "x")).Item1);

        Console.WriteLine(((P)new P(1, 2)).GetX());
        Console.WriteLine(((P)new P(1, 2)).Sum());
        Console.WriteLine(((P)new P(1, 2)).X);

        Console.WriteLine(((P)Make(5)).X);
        Console.WriteLine((Make(1)).Sum());
        Console.WriteLine((1, "x").Item2);
    }
}
