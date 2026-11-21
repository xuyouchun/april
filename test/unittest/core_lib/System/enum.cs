// System.Enum: the text of a value, Parse and TryParse (a name, a number, flags; the case), the class structure
// (enums are Enums and ValueTypes, structs are Structs and ValueTypes).

/* EXPECT

text Green 9 Minus A11 A3 Zero
parse Blue A10 3 Minus 7
try False True Green False
many 12 A0 A5 A11 A7
flags Read, None
generic Green Blue Red False Red
error bad bad bad
types True True True True True True

*/

import System;

using System;

enum Color { Red, Green = 5, Blue }

enum Mode { None = 0, Read = 1, Write = 2, Exec = 4 }

enum Sign : Int64 { Minus = -1, Zero, Plus }

enum Big { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11 }

struct Point { public Int32 X; }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("text " + Color.Green.ToString() + " " + ((Color)9).ToString() + " " + Sign.Minus + " " + Big.A11
                          + " " + ((Big)3) + " " + Sign.Zero);

        Console.WriteLine("parse " + Enum.Parse(typeof(Color), "Blue") + " " + Enum.Parse(typeof(Big), "a10", true) + " "
                          + Enum.Parse(typeof(Mode), "Read, Write") + " " + Enum.Parse(typeof(Sign), "-1") + " "
                          + Enum.Parse(typeof(Color), "7"));

        Object? r;
        Boolean a = Enum.TryParse(typeof(Color), "green", out r);
        Boolean b = Enum.TryParse(typeof(Color), "green", true, out r);
        Object? found = r;
        Boolean c = Enum.TryParse(typeof(Big), "A12", out r);
        Console.WriteLine("try " + a + " " + b + " " + found + " " + c);

        // More than eight items: by the hash tables.
        Console.WriteLine("many 12" + " " + Enum.Parse(typeof(Big), "A0") + " "
                          + Enum.Parse(typeof(Big), "a5", true) + " " + Enum.Parse(typeof(Big), " A11 ") + " "
                          + Enum.Parse(typeof(Big), "7"));

        Console.WriteLine("flags " + Enum.Parse(typeof(Mode), "Read") + ", "
                          + Enum.Parse(typeof(Mode), "none", true));

        Color g1 = Enum.Parse<Color>("Green"), g2, g3;
        Boolean g2ok = Enum.TryParse<Color>("blue", true, out g2);
        Boolean g3ok = Enum.TryParse<Color>("x", out g3);
        Console.WriteLine("generic " + g1 + " " + g2 + " " + Enum.Parse<Color>("red", true) + " " + g3ok + " " + g3);

        Int32 errors = 0;
        try { Enum.Parse(typeof(Color), "Nope"); } catch (ArgumentException e) { errors++; }
        try { Enum.Parse(typeof(Int32), "1"); } catch (ArgumentException e) { errors++; }
        try { Enum.Parse(typeof(Color), ""); } catch (ArgumentException e) { errors++; }
        Console.WriteLine("error " + (errors == 3? "bad bad bad" : "no"));

        Object color = Color.Red, point = new Point(), text = "s";
        Console.WriteLine("types " + (color is Enum) + " " + (color is ValueType) + " " + (point is Struct) + " "
                          + (point is ValueType) + " " + (point is Object) + " " + (text is Struct));
    }
}
