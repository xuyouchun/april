// A struct that can be null (P?): it is converted from a value and from null, passed, returned, kept in a field, compared with null,
// and read with Value; ?? gives the value when there is none.

/* EXPECT

7
-1
30
-1
True
7
True
3

*/

import System;

using System;

struct P { public Int32 x; public Int32 y; }

class Holder { public P? f; }

class Program
{
    static Int32 Sum(P? p)
    {
        if (p == null)
            return -1;

        P v = p.Value;
        return v.x + v.y;
    }

    static P? Make(Boolean b)
    {
        P p = new P();
        p.x = 10;
        p.y = 20;

        if (b)
            return p;

        return null;
    }

    [EntryPoint]
    public static void Main()
    {
        P p = new P();
        p.x = 3;
        p.y = 4;

        Console.WriteLine(Sum(p));
        Console.WriteLine(Sum(null));
        Console.WriteLine(Sum(Make(true)));
        Console.WriteLine(Sum(Make(false)));

        Holder h = new Holder();
        Console.WriteLine(h.f == null);
        h.f = p;
        Console.WriteLine(Sum(h.f));

        P? q = Make(true);
        q = null;
        Console.WriteLine(q == null);

        P d = Make(false) ?? p;
        Console.WriteLine(d.x);
    }
}
