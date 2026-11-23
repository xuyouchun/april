// A tuple literal (1, "2"): a struct (Tuple<..>) when the context wants one or does not tell, a reference (made by
// ITuple.Create, in the heap, not boxed) when the context wants a reference type.

/* EXPECT

struct 2 b
var 3
ref 1 two
long 9 t
nested 3 3
object 2
cast 6
call 7
call struct 4
return 3 r
return struct 4 four
field 4 field
ctor 5 six
lambda 8

*/

import System;

using System;

typedef MyTuple<T1, T2> = Tuple<Tuple<T1, T2>, Tuple<T1, T2>, Tuple<T1, T2>>;

class Pair
{
    public Int32 A;
    public String B;

    public Pair(Tuple<Int32, String> t)
    {
        this.A = t.Item1;
        this.B = t.Item2;
    }
}

class Holder
{
    public Tuple<Int32, String> Item = (4, "field");

    public static Int32 Take(ITuple<Int32, String> t) { return t.Item1 + 4; }

    public static Int32 TakeStruct(Tuple<Int32, String> t) { return t.Item1; }

    public static Int32 TakeObject(Object t) { return ((ITuple)t).GetSize(); }

    public static ITuple<Int32, String> Make() { return (3, "r"); }

    public static Tuple<Int32, String> MakeStruct() { return (4, "four"); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Tuple<Int32, String> a = (2, "b");
        Console.WriteLine("struct " + a.Item1 + " " + a.Item2);

        var b = (3, "three");
        Console.WriteLine("var " + b.Item1);

        ITuple<Int32, String> c = (1, "two");
        Console.WriteLine("ref " + c.Item1 + " " + c.Item2);

        Tuple<Int64, String> d = (9, "t");
        Console.WriteLine("long " + d.Item1 + " " + d.Item2);

        MyTuple<Int32, String> n = ((1, "one"), (2, "two"), (3, "three"));
        Console.WriteLine("nested " + n.Item3.Item1 + " " + n.Item1.Item2.Length);

        Object o = (1, "x");
        Console.WriteLine("object " + ((ITuple)o).GetSize());

        Tuple<Int32, String> k = (Tuple<Int32, String>)(6, "c");
        Console.WriteLine("cast " + k.Item1);

        Console.WriteLine("call " + Holder.Take((3, "t")));
        Console.WriteLine("call struct " + Holder.TakeStruct((4, "t")));

        ITuple<Int32, String> r = Holder.Make();
        Console.WriteLine("return " + r.Item1 + " " + r.Item2);
        Tuple<Int32, String> s = Holder.MakeStruct();
        Console.WriteLine("return struct " + s.Item1 + " " + s.Item2);

        Holder h = new Holder();
        Console.WriteLine("field " + h.Item.Item1 + " " + h.Item.Item2);

        Pair p = new Pair((5, "six"));
        Console.WriteLine("ctor " + p.A + " " + p.B);

        Func<Tuple<Int32, Int32>> f = () => (8, 9);
        Console.WriteLine("lambda " + f().Item1);
    }
}
