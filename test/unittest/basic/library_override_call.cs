// A method that overrides the one of a base type of a library is called through the derived type of the library.

/* EXPECT

direct 1 4 0 3
base 1 4 2 3
cast 120 120 65 7

*/

import System;

using System;
using System.Text;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        UTF8Encoding u8 = new UTF8Encoding();
        UTF32Encoding u32 = new UTF32Encoding(true);
        UnicodeEncoding u16 = new UnicodeEncoding(false);
        Console.WriteLine("direct " + u8.GetByteCount("x") + " " + u32.GetByteCount("x") + " " + u16.GetPreamble().Length
            + " " + new UTF8Encoding(true).GetPreamble().Length);

        Encoding b8 = u8, b32 = u32, b16 = u16;
        Console.WriteLine("base " + b8.GetByteCount("x") + " " + b32.GetByteCount("x") + " " + b16.GetByteCount("x")
            + " " + new UTF8Encoding(true).GetPreamble().Length);

        // Casts that follow each other.
        Int32 n = 120;
        Console.WriteLine("cast " + (Int32)(Char)(Int32)(n) + " " + (Int32)(Char)(Int32)(120) + " "
            + (Int32)(Char)(Int32)(65) + " " + (Int32)(Int16)(Int32)(7));
    }
}
