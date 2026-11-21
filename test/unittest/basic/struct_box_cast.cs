// (Object)x where x is a variable, a field or an array element of a custom struct: the struct is boxed (a copy).

/* EXPECT

local 5 6
argument 6 True True
field 7 8
element 9 10

*/

import System;

using System;

struct P
{
    public Int32 X;
    public P(Int32 x) { X = x; }

    public Boolean Same(Object other)
    {
        return other is P && ((P)other).X == X;
    }
}

class Holder
{
    public P Field = new P(7);
}

class Program
{
    static Int32 Read(Object o)
    {
        return ((P)o).X;
    }

    [EntryPoint]
    public static void Main()
    {
        P b = new P(5);
        Object o = (Object)b;
        b.X = 6;
        Console.WriteLine("local " + ((P)o).X + " " + b.X);

        Console.WriteLine("argument " + Read((Object)b) + " " + b.Same((Object)new P(6)) + " " + b.Same((Object)b));

        Holder h = new Holder();
        Object f = (Object)h.Field;
        h.Field.X = 8;
        Console.WriteLine("field " + ((P)f).X + " " + h.Field.X);

        P[] items = new P[2];
        items[1] = new P(9);
        Object e = (Object)items[1];
        items[1].X = 10;
        Console.WriteLine("element " + ((P)e).X + " " + items[1].X);
    }
}
