// A tuple literal can name its items: (Number: 1, Str: "two"). A var that is given it has the names; a variable that is
// declared with the names (or a parameter) has its own, the names of the literal are the names of the items that it is made of.

/* EXPECT

1two
7
9
1

*/

import System;
using System;

class Program
{
    static Int32 Pick(ITuple<Int32, String> t) { return t.Item1; }

    [EntryPoint]
    public static void Main()
    {
        var t = (Number: 1, Str: "two");
        Console.WriteLine(t.Number + t.Str);
        (Int32 A, Int32 B) p = (A: 3, B: 4);
        Console.WriteLine(p.A + p.B);
        Console.WriteLine(Pick((Id: 9, Name: "n")));
        var c = 5 > 3 ? 1 : 2;
        Console.WriteLine(c);
    }
}
