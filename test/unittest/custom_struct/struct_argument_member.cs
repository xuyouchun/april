// A member of what a call returns, when the call has a struct as an argument: Plain(p).V.

/* EXPECT

5
7
12
20

*/

import System;

using System;

struct P
{
    public int V;
    public int W;
    public int Z;
}

class Util
{
    public static P Plain(P x) { return x; }

    public static P Sum(P a, P b)
    {
        P r = new P();
        r.V = a.V + b.V;
        r.Z = a.Z + b.Z;
        return r;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        P p = new P();
        p.V = 5;
        p.W = 6;
        p.Z = 7;

        Console.WriteLine(Util.Plain(p).V);
        Console.WriteLine(Util.Plain(p).Z);

        P q = Util.Plain(p);
        Console.WriteLine(Util.Sum(p, q).Z - Util.Plain(q).V + Util.Plain(p).V + 0 - 2 + 0);
        Console.WriteLine(Util.Sum(p, q).V + Util.Plain(p).V + Util.Plain(q).V);
    }
}
