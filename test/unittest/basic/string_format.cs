// String.Format and StringBuilder.AppendFormat: {index[,alignment][:format]}, the braces, and the formats of the numbers
// (IFormattable: D, X, B, F, N, E, G).

/* EXPECT

plain 1 a True x
width    42|42   |   ab|ab   |
braces {0} } {
null [] [x]
integers 00042 -00042 42 007 ff 00FF FF FFFFFFFF FFFFFFFFFFFFFFFF
binary 101 00000101 11111111 1111111111111111
fixed 42.00 -7.0 1234567 12.5000
group 1,234,567.00 1,234,567 999 -1,234.5 1,000.000 0.00
doubles 3.14 2 3 0.12 0.13 1.5 100000000000000000000
scientific 1.234568E+003 1.235E+003 0.00E+000 -1.5e-002 1E+000
special NaN Infinity -Infinity NaN
single 1.2 1.5 3.14
direct ff 3.5 00C8 1234.57
append a-1-A b 7
caught caught caught caught caught caught caught caught
mixed 5 True x 2.5 abc 9

*/

import System;

using System;
using System.Text;

class Program
{
    static String Try(String format, Object arg)
    {
        try
        {
            return String.Format(format, arg);
        }
        catch (FormatException)
        {
            return "caught";
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("plain " + String.Format("{0} {1}", 1, "a") + " " + String.Format("{0}", true) + " " + String.Format("{0}", 'x'));

        Console.WriteLine("width " + String.Format("{0,5}|{0,-5}|{1,5}|{1,-5}|", 42, "ab"));

        Console.WriteLine("braces " + String.Format("{{0}}") + " " + String.Format("}}") + " " + String.Format("{{"));

        String empty = null;
        Console.WriteLine("null [" + String.Format("{0}", empty) + "] [" + String.Format("{0}", "x") + "]");

        Console.WriteLine("integers " + String.Format("{0:D5}", 42) + " " + String.Format("{0:D5}", -42) + " " + String.Format("{0:G}", 42)
            + " " + String.Format("{0:G3}", 7) + " " + String.Format("{0:x}", 255) + " " + String.Format("{0:X4}", 255)
            + " " + String.Format("{0:X}", (Int8)(-1)) + " " + String.Format("{0:X}", -1) + " " + String.Format("{0:X}", -1L));

        Console.WriteLine("binary " + String.Format("{0:B}", 5) + " " + String.Format("{0:B8}", 5) + " " + String.Format("{0:B}", (UInt8)255)
            + " " + String.Format("{0:b}", (Int16)(-1)).Substring(0, 16));

        Console.WriteLine("fixed " + String.Format("{0:F}", 42) + " " + String.Format("{0:F1}", -7) + " " + String.Format("{0:F0}", 1234567)
            + " " + String.Format("{0:F4}", 12.5));

        Console.WriteLine("group " + String.Format("{0:N}", 1234567) + " " + String.Format("{0:N0}", 1234567) + " " + String.Format("{0:N0}", 999)
            + " " + String.Format("{0:N1}", -1234.5) + " " + String.Format("{0:N3}", 1000.0) + " " + String.Format("{0:N}", 0.001));

        Console.WriteLine("doubles " + String.Format("{0:F2}", 3.14159) + " " + String.Format("{0:F0}", 2.5) + " " + String.Format("{0:F0}", 2.51)
            + " " + String.Format("{0:F2}", 0.125) + " " + String.Format("{0:F2}", 0.126) + " " + String.Format("{0}", 1.5)
            + " " + String.Format("{0:F0}", 1e20));

        Console.WriteLine("scientific " + String.Format("{0:E}", 1234.5678) + " " + String.Format("{0:E3}", 1234.5678) + " " + String.Format("{0:E2}", 0.0)
            + " " + String.Format("{0:e1}", -0.015) + " " + String.Format("{0:E0}", 1.0));

        Double zero = 0.0, nan = zero / zero;
        Console.WriteLine("special " + String.Format("{0:F2}", nan) + " " + String.Format("{0:F2}", 1.0 / zero) + " " + String.Format("{0:N1}", -1.0 / zero)
            + " " + String.Format("{0:E3}", nan));

        Single half = 1.25f;
        Console.WriteLine("single " + String.Format("{0:F1}", half) + " " + String.Format("{0:F1}", 1.5f) + " " + String.Format("{0:F2}", 3.14159f));

        Int32 hex = 255;
        Double fraction = 3.5, price = 1234.5678;
        UInt8 byte200 = 200;
        Console.WriteLine("direct " + hex.ToString("x") + " " + fraction.ToString("F1") + " " + byte200.ToString("X4") + " " + price.ToString("F2"));

        StringBuilder sb = new StringBuilder("a");
        sb.AppendFormat("-{0}-{1:X}", 1, 10).Append(' ').AppendFormat("{0}", "b");
        Console.WriteLine("append " + sb + " " + sb.Length);

        Console.WriteLine("caught " + Try("{0", 1) + " " + Try("}", 1) + " " + Try("{x}", 1) + " " + Try("{1}", 1) + " " + Try("{0:Q}", 1)
            + " " + Try("{0,}", 1) + " " + Try("{0:D}x{", 1));

        Console.WriteLine("mixed " + String.Format("{0}{1}{2}", 5, true, 'x').Substring(0, 1) + " " + String.Format("{0}", true) + " " + String.Format("{0}", "x")
            + " " + String.Format("{0:F1}", 2.5) + " " + String.Format("{0}{1}", "ab", 'c') + " " + String.Format("{0}", 9L));
    }
}
