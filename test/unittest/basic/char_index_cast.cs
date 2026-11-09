// A Char that is read by an index (an array element, the indexer of a string, the indexer of a class) is
// converted to the type of the variable that takes it, in the interpreter, the JIT and the native programs.

/* EXPECT

97 233 20013
97 233 98
120 233 99
7 9 99
2.5 True True

*/

import System;

using System;

class Holder
{
    public Char this[Int32 k] { get { return 'c'; } }
    public Int16 Small { get { return (Int16)7; } }
}

class Program
{
    static Int32 FromArray(Char[] a, Int32 k)
    {
        Int32 c = (Int32)a[k];
        return c;
    }

    static Int32 FromString(String s, Int32 k)
    {
        Int32 c = (Int32)s[k];
        return c;
    }

    static Int64 Wide(String s, Int32 k)
    {
        Int64 c = (Int64)s[k];
        return c;
    }

    static Double Real(Char[] a)
    {
        Double d = (Double)a[0];
        return d;
    }

    static Int32 FromHolder(Holder h)
    {
        Int32 c = (Int32)h[0];
        return c;
    }

    [EntryPoint]
    public static void Main()
    {
        Char[] a = new Char[] { 'a', 'é', '中' };
        Console.WriteLine(FromArray(a, 0) + " " + FromArray(a, 1) + " " + FromArray(a, 2));

        Console.WriteLine(FromString("aé", 0) + " " + FromString("aé", 1) + " " + (Int32)("xb"[1]));
        Console.WriteLine(Wide("xé", 0) + " " + Wide("xé", 1) + " " + FromHolder(new Holder()));

        Holder h = new Holder();
        Int32 small = (Int32)h.Small;
        Int32[] items = new Int32[] { 9 };
        Console.WriteLine(small + " " + items[0] + " " + (Int32)h[0] + "");

        Console.WriteLine((Real(new Char[] { '\u0002' }) + 0.5) + " " + (Real(new Char[] { 'd' }) - 0.0 > 99) + " " + (FromString("a", 0) == 97));
    }
}
