// default is default(Type) of the type that the context wants: a variable, an assignment, a return, an argument, a cast,
// an element of an array, the other side of ?? and ?: .

/* EXPECT

0 0 null False
assign 0 null
return 0 null False
arg 0 null
cast 0
array 0 0
coalesce 0
cond 0 1
struct 0 0
field 0 null
param 0n 3n
compare False True

*/

import System;

using System;

struct Pair
{
    public Int32 X;
    public Int32 Y;
}

class Holder
{
    public Int32 Number = default;
    public String Text = default;
}

class Program
{
    static Int32 Int() { return default; }

    static String Str() { return default; }

    static Boolean Bool() { return default; }

    static String Opt(Int32 x = default, String s = default) { return x + (s == null? "n" : s); }

    static String Show(Int32 n, String s) { return n + " " + (s == null? "null" : s); }

    [EntryPoint]
    public static void Main()
    {
        Int32 a = default;
        Double d = default;
        String s = default;
        Boolean b = default;
        Console.WriteLine(a + " " + (Int32)d + " " + (s == null? "null" : s) + " " + b);

        a = 5;
        s = "x";
        a = default;
        s = default;
        Console.WriteLine("assign " + a + " " + (s == null? "null" : s));

        Console.WriteLine("return " + Int() + " " + (Str() == null? "null" : Str()) + " " + Bool());

        Console.WriteLine("arg " + Show(default, default));

        Console.WriteLine("cast " + (Int32)default(Int32));

        Int32[] numbers = new Int32[] { default, default };
        Console.WriteLine("array " + numbers[0] + " " + numbers[1]);

        Int32? maybe = null;
        Console.WriteLine("coalesce " + (maybe ?? default));

        Console.WriteLine("cond " + (a == 0? default : 1) + " " + (a == 1? default : 1));

        Pair pair = default;
        Console.WriteLine("struct " + pair.X + " " + pair.Y);

        Holder holder = new Holder();
        Console.WriteLine("field " + holder.Number + " " + (holder.Text == null? "null" : holder.Text));

        Console.WriteLine("param " + Opt() + " " + Opt(3));

        Int32 four = 4;
        Console.WriteLine("compare " + (four == default) + " " + (four != default));
    }
}
