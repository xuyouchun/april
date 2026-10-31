// Nullable types: T?

/* EXPECT

True
5
False
8
False
3
-1
8
8
False
10
7
False
False
True
True
True
8
100
8
no value
6
has
2.5
True
False
7
99
c
-1

*/

import System;

using System;

class Holder
{
    public int? Count;
    public double? Ratio;
}

class Program
{
    static int Get(int? v) { return v.GetValueOrDefault(-1); }

    static int? Find(int k)
    {
        if (k > 0)
            return k * 2;

        return null;
    }

    static int? Id(int? v) { return v; }

    [EntryPoint]
    public static void Main()
    {
        int? a = 5;
        Console.WriteLine(a.HasValue);
        Console.WriteLine(a.Value);

        a = null;
        Console.WriteLine(a.HasValue);

        a = 8;
        Console.WriteLine(a.Value);

        int? b = null;
        Console.WriteLine(b.HasValue);

        // Arguments and results.
        Console.WriteLine(Get(3));
        Console.WriteLine(Get(null));
        Console.WriteLine(Get(a));

        int? c = Find(4);
        Console.WriteLine(c.Value + 0 - 0);
        int? d = Find(0);
        Console.WriteLine(d.HasValue);
        Console.WriteLine(Find(5).Value);
        Console.WriteLine(Id(7).Value);
        Console.WriteLine(Id(null).HasValue);

        // Comparison with null.
        Console.WriteLine(c == null);
        Console.WriteLine(c != null);
        Console.WriteLine(d == null);
        Console.WriteLine(null == d);

        // ??, casts.
        Console.WriteLine(c ?? 100 - 92);
        Console.WriteLine(d ?? 100);
        int x = (int)c;
        Console.WriteLine(x);
        try { int y = (int)d; Console.WriteLine(y); }
        catch (InvalidOperationException) { Console.WriteLine("no value"); }

        int? e = (int?)6;
        Console.WriteLine(e.Value);

        if (c != null)
            Console.WriteLine("has");

        // Fields, other types.
        Holder h = new Holder();
        h.Ratio = 2.5;
        Console.WriteLine(h.Ratio.Value);
        Console.WriteLine(h.Ratio.HasValue);
        Console.WriteLine(h.Count.HasValue);
        h.Count = 7;
        Console.WriteLine(h.Count.Value);

        long? big = 99;
        Console.WriteLine(big.Value);

        char? ch = 'c';
        Console.WriteLine(ch.Value);

        int? none = null;
        Console.WriteLine(none.GetValueOrDefault(-1));
    }
}
