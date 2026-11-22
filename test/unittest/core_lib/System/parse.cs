// Parse and TryParse of the numbers and of Boolean, and Object.GetType().

/* EXPECT

int 42 -7 0 15
int8 127 -128
int16 -32768 32767
int64 9223372036854775807 -9223372036854775808
uint 255 65535 4294967295 18446744073709551615
neg-zero 0
bad-int False False False False False False
overflow int32 True True
overflow uint False False
overflow int64 False True
overflow caught OverflowException
format caught FormatException
null caught ArgumentException
double 3.5 -0.25 1000 0.015 12
double special True True True
double text False False False False
single 1.5 100
precise 0.1 True
big True True
bool True False False False
culture 3,5 3.5
type Exception InvalidOperationException
type Int32 String Program.Thing
same True False
done

*/

import System;

using System;
using System.Globalization;
using System.Reflection;

class Program
{
    class Thing
    {
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("int " + Int32.Parse("42") + " " + Int32.Parse(" -7 ") + " " + Int32.Parse("+0") + " " + Int32.Parse("0015"));
        Console.WriteLine("int8 " + Int8.Parse("127") + " " + Int8.Parse("-128"));
        Console.WriteLine("int16 " + Int16.Parse("-32768") + " " + Int16.Parse("32767"));
        Console.WriteLine("int64 " + Int64.Parse("9223372036854775807") + " " + Int64.Parse("-9223372036854775808"));
        Console.WriteLine("uint " + UInt8.Parse("255") + " " + UInt16.Parse("65535") + " " + UInt32.Parse("4294967295") + " "
                          + UInt64.Parse("18446744073709551615"));
        Console.WriteLine("neg-zero " + UInt32.Parse("-0"));

        Int32 n;
        Console.WriteLine("bad-int " + Int32.TryParse("", out n) + " " + Int32.TryParse("12a", out n) + " " + Int32.TryParse("1 2", out n)
                          + " " + Int32.TryParse("-", out n) + " " + Int32.TryParse("0x10", out n) + " " + Int32.TryParse(null, out n));

        Int8 b8;
        UInt8 u8;
        UInt32 u32;
        Int64 i64;
        Console.WriteLine("overflow int32 " + Int32.TryParse("2147483647", out n) + " " + !Int32.TryParse("2147483648", out n));
        Console.WriteLine("overflow uint " + UInt8.TryParse("256", out u8) + " " + UInt32.TryParse("-1", out u32));
        Console.WriteLine("overflow int64 " + Int64.TryParse("9223372036854775808", out i64) + " " + Int64.TryParse("-9223372036854775808", out i64));

        try { Int8.Parse("128"); }
        catch (OverflowException e) { Console.WriteLine("overflow caught OverflowException"); }

        try { Int32.Parse("x"); }
        catch (FormatException e) { Console.WriteLine("format caught FormatException"); }

        try { Int32.Parse(null); }
        catch (ArgumentException e) { Console.WriteLine("null caught ArgumentException"); }

        Console.WriteLine("double " + Double.Parse("3.5") + " " + Double.Parse("-0.25") + " " + Double.Parse("1e3") + " " + Double.Parse("1.5E-2")
                          + " " + Double.Parse(" 12. "));

        Double nan = Double.Parse("NaN");
        Double inf = Double.Parse("Infinity");
        Double ninf = Double.Parse("-Infinity");
        Console.WriteLine("double special " + (nan != nan) + " " + (inf > 1e308) + " " + (ninf < -1e308));

        Double d;
        Console.WriteLine("double text " + Double.TryParse("", out d) + " " + Double.TryParse("1e", out d) + " " + Double.TryParse("1.2.3", out d)
                          + " " + Double.TryParse("abc", out d));

        Console.WriteLine("single " + Single.Parse("1.5") + " " + Single.Parse("1e2"));

        // The nearest number: what ToString gives parses back to the same one.
        Double tenth = Double.Parse("0.1");
        Console.WriteLine("precise " + tenth + " " + (Double.Parse(tenth.ToString()) == tenth));
        Console.WriteLine("big " + (Double.Parse("1e999") > 1e308) + " " + (Double.Parse("1e-999") == 0));

        Boolean flag;
        Console.WriteLine("bool " + Boolean.Parse(" TRUE ") + " " + Boolean.Parse("false") + " " + Boolean.TryParse("yes", out flag)
                          + " " + Boolean.TryParse(null, out flag));

        NumberFormatInfo comma = new NumberFormatInfo();
        comma.NumberDecimalSeparator = ",";
        Console.WriteLine("culture " + Double.Parse("3,5", comma).ToString(null, comma) + " " + Double.Parse("3.5", NumberFormatInfo.InvariantInfo));

        Exception error = new InvalidOperationException("x");
        Console.WriteLine("type " + typeof(Exception).Name + " " + error.GetType().Name);

        Object text = "a";
        Object number = 5;
        Object thing = new Thing();
        Console.WriteLine("type " + number.GetType().Name + " " + text.GetType().Name + " " + thing.GetType().Name);
        Console.WriteLine("same " + Object.ReferenceEquals(error.GetType(), typeof(InvalidOperationException)) + " " + Object.ReferenceEquals(error.GetType(), typeof(Exception)));
        Console.WriteLine("done");
    }
}
