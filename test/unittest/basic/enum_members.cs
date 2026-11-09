// An enum can have methods, properties, operators, const fields and implement interfaces: the items end with ';' when
// there are members. `this` is the value of the enum.

/* EXPECT

False True
c2 c2 c2
c3 c3 c1
20 c2 5
Green Blue
done

*/

import System;

using System;

interface ILabel
{
    String Label();
}

enum Color : Int32, ILabel
{
    Red = 1, Green, Blue;

    public const Int32 Limit = 5;

    public Boolean IsWarm => this == Red;

    public String Label() { return "c" + (Int32)this; }

    public static Color Parse(String s) { return s == "g"? Green : Red; }

    public Color Next() { return this == Blue? Red : (Color)((Int32)this + 1); }

    // The cast of the language is used between an enum and a number, the implicit operator where there is no cast.
    public static implicit operator Int32(Color c) { return (Int32)c * 10; }
}

// The underlying type is not written when no interface is implemented; the members follow the items.
enum Level : UInt8
{
    Low, High;

    public String Name { get { return this == Low? "Green" : "Blue"; } }
}

class Program
{
    static String Show<T>(T v) where T : ILabel { return v.Label(); }

    [EntryPoint]
    public static void Main()
    {
        Color c = Color.Green;
        Console.WriteLine(c.IsWarm + " " + Color.Red.IsWarm);
        Console.WriteLine(c.Label() + " " + Show(c) + " " + Color.Parse("g").Label());
        Console.WriteLine(Color.Blue.Label() + " " + c.Next().Label() + " " + Color.Blue.Next().Label());

        Int32 n = c;
        ILabel il = c;
        Console.WriteLine(n + " " + il.Label() + " " + Color.Limit);

        Console.WriteLine(Level.Low.Name + " " + Level.High.Name);
        Console.WriteLine("done");
    }
}
