// A property that is the object of a member, Property.Member or Property.Method(), in the members of its own class or struct: it is
// read from this (it was the object of itself, and the compiler overflowed its stack).

/* EXPECT

2 AB 2 5
len 2 upper AB count 2
struct 05 22 12
static 3 x

*/

import System;

using System;

class Holder
{
    public Holder(String v) { __v = v; }

    public String Name => __v;

    public Int32 Count { get { return __v.Length; } }

    public Int32 Len() { return Name.Length; }

    public String Upper() { return Name.ToUpper(); }

    public String CountText() { return Count.ToString(); }

    public String Both() { return "len " + Name.Length + " upper " + Name.ToUpper() + " count " + Count.ToString(); }

    public Int32 Sum() { return Name.Length + Count + Name.Substring(1).Length + Count.GetHashCode() - Count; }

    private readonly String __v;
}

struct Month
{
    public Month(Int32 v) { __v = v; }

    public Int32 Value => __v;

    public String Text() { return Value.ToString("D2"); }

    public String Pair() { return Value.ToString() + Value.ToString(); }

    public String Next() { return (Value + 1).ToString() + (Value + 2).ToString(); }

    public String Add() { return (Value + 7).ToString(); }

    private readonly Int32 __v;
}

class Statics
{
    public static Int32 Number => 3;

    public static String Label => "x";

    public static String Text() { return Number.ToString() + " " + Label.ToString(); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Holder h = new Holder("ab");
        Console.WriteLine(h.Len() + " " + h.Upper() + " " + h.CountText() + " " + h.Sum());
        Console.WriteLine(h.Both());

        Month m = new Month(5);
        Console.WriteLine("struct " + m.Text() + " " + m.Pair().Length + m.Next().Length + " " + m.Add());

        Console.WriteLine("static " + Statics.Text());
    }
}
