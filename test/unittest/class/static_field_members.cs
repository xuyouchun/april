// The members of a static field that is read in a static method: table.Length, text.Length, table[k].

/* EXPECT

array 3 4 0
nullable 3
string 3 4
struct 6
generic 2
loop 18
instance 5

*/

import System;

using System;

class Tables
{
    private static Int32[] __table = new Int32[4];

    private static Int32[]? __lazy;

    private static String __text = "abc";

    public static Int32 Length() => __table.Length;

    public static Int32 First() => __table[0];

    public static Int32 Lazy()
    {
        if (__lazy == null)
            __lazy = new Int32[3];

        return __lazy.Length;
    }

    public static Int32 TextLength() => __text.Length;

    public static Int32 Sum()
    {
        for (Int32 k = 0; k < __table.Length; k++)
            __table[k] = k + 3;

        Int32 total = 0;
        for (Int32 k = 0; k < __table.Length; k++)
            total += __table[k];

        return total;
    }
}

struct Holder
{
    private static Int32[]? __items;

    public static Int32 Count()
    {
        __items = new Int32[6];
        return __items.Length;
    }
}

class Generic<T>
{
    private static T[]? __items;

    public static Int32 Count()
    {
        __items = new T[2];
        return __items.Length;
    }
}

class Instance
{
    private Int32[] __items = new Int32[5];

    public Int32 Count() => __items.Length;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("array " + Tables.Lazy() + " " + Tables.Length() + " " + Tables.First());
        Console.WriteLine("nullable " + Tables.Lazy());
        Console.WriteLine("string " + Tables.TextLength() + " " + Tables.Length());
        Console.WriteLine("struct " + Holder.Count());
        Console.WriteLine("generic " + Generic<Int32>.Count());
        Console.WriteLine("loop " + Tables.Sum());
        Console.WriteLine("instance " + new Instance().Count());
    }
}
