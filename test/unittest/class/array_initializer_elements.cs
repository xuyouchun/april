// The initializer of an array: the elements that are new T(arguments) take the variables as arguments, and the
// elements that are structs are written in the array (a struct, a call, a generic param, a variable).

/* EXPECT

class local 3 5
struct local 3 4
struct literal 1 2 3
struct default 0 2
struct variable 7 7
string 5 a3
return 5 6 2
generic struct 2 3
generic int 3 4
object boxed 7 s
param 6
call 4 5
nested 4 3
empty 0
in expression 2

*/

import System;

using System;

struct P
{
    public P(Int32 x, Int32 y)
    {
        X = x;
        Y = y;
    }

    public Int32 X;

    public Int32 Y;
}

struct Q
{
    public Int32 Z;
}

class C
{
    public C(Int32 x)
    {
        X = x;
    }

    public Int32 X;
}

class K
{
    public static P[] Make(Int32 n) => new P[] { new P(n, 1), new P(n + 1, 2) };

    public static T[] Pair<T>(T a, T b) => new T[] { a, b };

    public static P Mk(Int32 v) => new P(v, v);

    public static Int32 Sum(P[] items)
    {
        Int32 total = 0;

        for (Int32 k = 0; k < items.Length; k++)
            total += items[k].X;

        return total;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 3;
        C[] classes = new C[] { new C(n), new C(5) };
        Console.WriteLine("class local " + classes[0].X + " " + classes[1].X);

        P[] local = new P[] { new P(n, 4) };
        Console.WriteLine("struct local " + local[0].X + " " + local[0].Y);

        P[] literal = new P[] { new P(1, 2), new P(3, 4) };
        Console.WriteLine("struct literal " + literal[0].X + " " + literal[0].Y + " " + literal[1].X);

        Q[] defaults = new Q[] { new Q(), new Q() };
        Console.WriteLine("struct default " + defaults[0].Z + " " + defaults.Length);

        P p = new P(7, 8);
        P[] variables = new P[] { p, p };
        Console.WriteLine("struct variable " + variables[0].X + " " + variables[1].X);

        String[] texts = new String[] { "a" + n, "b" };
        Console.WriteLine("string " + (n + 2) + " " + texts[0]);

        P[] made = K.Make(5);
        Console.WriteLine("return " + made[0].X + " " + made[1].X + " " + made[1].Y);

        P[] pair = K.Pair(new P(1, 2), new P(3, 4));
        Console.WriteLine("generic struct " + pair[0].Y + " " + pair[1].X);

        Int32[] numbers = K.Pair(3, 4);
        Console.WriteLine("generic int " + numbers[0] + " " + numbers[1]);

        Object[] boxed = new Object[] { p, "s" };
        Console.WriteLine("object boxed " + ((P)boxed[0]).X + " " + boxed[1]);

        Console.WriteLine("param " + K.Sum(new P[] { new P(1, 0), new P(2, 0), new P(3, 0) }));

        P[] calls = new P[] { K.Mk(4), K.Mk(5) };
        Console.WriteLine("call " + calls[0].X + " " + calls[1].Y);

        P[] nested = new P[] { new P(n + 1, n * 2) };
        Console.WriteLine("nested " + nested[0].X + " " + (nested[0].Y / 2));

        P[] none = new P[] { };
        Console.WriteLine("empty " + none.Length);

        Console.WriteLine("in expression " + new P[] { new P(1, 2), new P(3, 4) }.Length);
    }
}
