// x == null and x != null of a value of a generic param (no constraint): a reference is null or not, a value (a number, a struct)
// is boxed, it is not null.

/* EXPECT

True
False
True
False
False
True
False
True
False
False
1
2
nothing
something

*/

import System;

using System;

class Pt { public Int32 X; }

struct S { public Int32 X; }

class Box<T>
{
    public T Value;

    public Boolean IsNull() { return Value == null; }

    public Boolean NotNull() { return Value != null; }

    public Boolean IsNullRev() { return null == Value; }

    public String Describe() { return Value == null? "nothing" : "something"; }
}

class Program
{
    static Boolean Check<T>(T v) { return v == null; }

    static Int32 Pick<T>(T v)
    {
        if (v == null)
            return 1;

        return 2;
    }

    [EntryPoint]
    public static void Main()
    {
        Box<String> b = new Box<String>();
        Console.WriteLine(b.IsNull());
        b.Value = "x";
        Console.WriteLine(b.IsNull());
        Console.WriteLine(b.NotNull());
        Console.WriteLine(b.IsNullRev());

        Box<Int32> bi = new Box<Int32>();
        Console.WriteLine(bi.IsNull());

        Box<Pt> bp = new Box<Pt>();
        Console.WriteLine(bp.IsNull());
        bp.Value = new Pt();
        Console.WriteLine(bp.IsNull());

        Console.WriteLine(Check<String>(null));
        Console.WriteLine(Check<Int32>(3));
        Console.WriteLine(Check<S>(new S()));
        Console.WriteLine(Pick<Pt>(null));
        Console.WriteLine(Pick<Pt>(new Pt()));

        Box<Pt> empty = new Box<Pt>();
        Console.WriteLine(empty.Describe());
        empty.Value = new Pt();
        Console.WriteLine(empty.Describe());
    }
}
