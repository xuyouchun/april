// ToString of objects, and objects in the concatenation.

/* EXPECT

P(3)
v=P(3)
P(3)!
o=P(3)
n=4

*/

import System;

using System;
class P
{
    public int X;
    public P(int x)
    {
        X = x;
    }

    public override String ToString()
    {
        return "P(" + X + ")";
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        P p = new P(3);
        Console.WriteLine(p.ToString());
        Console.WriteLine("v=" + p);
        Console.WriteLine(p + "!");
        Object o = p;
        Console.WriteLine("o=" + o);
        String s = "n=" + 4;
        Console.WriteLine(s);
    }
}
