// The text of an enum value: the name of its item, the number when the value has no item; an enum that has a ToString of its
// own keeps it.

/* EXPECT

Green Blue Red
High Minus Plus
9 7 3
text Green Green Low Zero
named-1 named-0
True True Green named-0 High

*/

import System;

using System;

enum Color { Red, Green = 5, Blue }

enum Level : UInt8 { Low = 1, High = 200 }

enum Sign : Int64 { Minus = -1, Zero, Plus }

enum Mode { None = 0, Read = 1, Write = 2 }

enum Named
{
    A, B;

    public override String ToString() { return "named-" + (Int32)this; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Color c = Color.Green;
        Console.WriteLine(c.ToString() + " " + Color.Blue.ToString() + " " + Color.Red.ToString());
        Console.WriteLine(Level.High.ToString() + " " + Sign.Minus.ToString() + " " + Sign.Plus.ToString());

        // No item: the number.
        Console.WriteLine(((Color)9).ToString() + " " + ((Level)7).ToString() + " " + (Mode.Read | Mode.Write).ToString());

        // A boxed value, the concatenation.
        Object o = c;
        Console.WriteLine("text " + c + " " + o + " " + Object.ToString(Level.Low) + " " + Sign.Zero);

        // The ToString of the enum.
        Console.WriteLine(Named.B.ToString() + " " + Object.ToString(Named.A));

        // Every enum is a System.Enum: the ToString of it is the one of System.Enum, or the one that the enum overrides.
        Object boxed = Level.High;
        Enum asEnum = c;
        Enum named = Named.A;
        Console.WriteLine((boxed is Enum) + " " + (boxed as Enum != null) + " " + asEnum.ToString() + " " + named.ToString()
                          + " " + ((Enum)boxed).ToString());
    }
}
