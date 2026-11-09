// A generic method that takes the type of the argument is better than a method that converts it
// (boxing); a method of which the constraints do not fit is not a candidate.

/* EXPECT

S[x]
C[c]
G[12]
G[12]
G[2.5]
G[True]
G[box]
O

*/

import System;

using System;

class Box : IStringable
{
    public String ToString() => "box";
}

class Plain { }

class Sink
{
    public Sink Add(String? v) { Console.WriteLine("S[" + v + "]"); return this; }
    public Sink Add(Char v) { Console.WriteLine("C[" + v + "]"); return this; }
    public Sink Add(Object? v) { Console.WriteLine("O"); return this; }
    public Sink Add<T>(T v) where T : IStringable { Console.WriteLine("G[" + v.ToString() + "]"); return this; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Sink s = new Sink();
        s.Add("x").Add('c').Add(12).Add(12L).Add(2.5).Add(true).Add(new Box()).Add(new Plain());
    }
}
