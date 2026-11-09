// Encoding.GetEncoding: the encodings by name and by code page, the properties of an encoding, Convert.

/* EXPECT

names True True True True True True
aliases True True True True True True True
case True True True
pages True True True True True True
props utf-8 65001 Unicode (UTF-8) False | utf-16 1200 | utf-16BE 1201 | utf-32 12000 | us-ascii 20127 True | iso-8859-1 28591 True
display Unicode Unicode (Big-Endian) Unicode (UTF-32) US-ASCII Western European (ISO)
unknown caught caught caught
fallbacks True True True | 3f e9
gb True True True 54936 gb18030
default True
convert 61 | c3 a9 | e9 | e9 00 | 61 3f
range caught
kept True False

*/

import System;

using System;
using System.Text;

class Program
{
    static String Hex(UInt8[] bytes)
    {
        StringBuilder sb = new StringBuilder();
        String digits = "0123456789abcdef";

        for (Int32 k = 0; k < bytes.Length; k++)
        {
            Int32 b = (Int32)bytes[k];
            if (k > 0)
                sb.Append(' ');

            sb.Append(digits[b >> 4]).Append(digits[b & 15]);
        }

        return sb.ToString();
    }

    static Boolean Same(Object a, Object b) => Object.ReferenceEquals(a, b);

    [EntryPoint]
    public static void Main()
    {
        // The encodings of the names are the ones of the properties.
        Console.WriteLine("names " + Same(Encoding.GetEncoding("utf-8"), Encoding.UTF8) + " " + Same(Encoding.GetEncoding("utf-16"), Encoding.Unicode)
            + " " + Same(Encoding.GetEncoding("utf-16BE"), Encoding.BigEndianUnicode) + " " + Same(Encoding.GetEncoding("utf-32"), Encoding.UTF32)
            + " " + Same(Encoding.GetEncoding("us-ascii"), Encoding.ASCII) + " " + Same(Encoding.GetEncoding("iso-8859-1"), Encoding.Latin1));

        Console.WriteLine("aliases " + Same(Encoding.GetEncoding("utf8"), Encoding.UTF8) + " " + Same(Encoding.GetEncoding("unicode"), Encoding.Unicode)
            + " " + Same(Encoding.GetEncoding("UnicodeFFFE"), Encoding.BigEndianUnicode) + " " + Same(Encoding.GetEncoding("utf32"), Encoding.UTF32)
            + " " + Same(Encoding.GetEncoding("ascii"), Encoding.ASCII) + " " + Same(Encoding.GetEncoding("latin1"), Encoding.Latin1)
            + " " + Same(Encoding.GetEncoding("ISO8859-1"), Encoding.Latin1));

        Console.WriteLine("case " + Same(Encoding.GetEncoding("UTF-8"), Encoding.UTF8) + " " + Same(Encoding.GetEncoding("  Utf-8 "), Encoding.UTF8)
            + " " + Same(Encoding.GetEncoding("US-ASCII"), Encoding.ASCII));

        Console.WriteLine("pages " + Same(Encoding.GetEncoding(65001), Encoding.UTF8) + " " + Same(Encoding.GetEncoding(1200), Encoding.Unicode)
            + " " + Same(Encoding.GetEncoding(1201), Encoding.BigEndianUnicode) + " " + Same(Encoding.GetEncoding(12000), Encoding.UTF32)
            + " " + Same(Encoding.GetEncoding(20127), Encoding.ASCII) + " " + Same(Encoding.GetEncoding(28591), Encoding.Latin1));

        Encoding u8 = Encoding.UTF8, u16 = Encoding.Unicode, be = Encoding.BigEndianUnicode, u32 = Encoding.UTF32;
        Console.WriteLine("props " + u8.WebName + " " + u8.CodePage + " " + u8.EncodingName + " " + u8.IsSingleByte
            + " | " + u16.WebName + " " + u16.CodePage + " | " + be.WebName + " " + be.CodePage + " | " + u32.WebName + " " + u32.CodePage
            + " | " + Encoding.ASCII.WebName + " " + Encoding.ASCII.CodePage + " " + Encoding.ASCII.IsSingleByte
            + " | " + Encoding.Latin1.WebName + " " + Encoding.Latin1.CodePage + " " + Encoding.Latin1.IsSingleByte);

        Console.WriteLine("display " + u16.EncodingName + " " + be.EncodingName + " " + u32.EncodingName + " " + Encoding.ASCII.EncodingName
            + " " + Encoding.Latin1.EncodingName);

        String r1 = "no", r2 = "no", r3 = "no";
        try { Encoding.GetEncoding("klingon"); }
        catch (ArgumentException) { r1 = "caught"; }

        try { Encoding.GetEncoding(437); }
        catch (ArgumentException) { r2 = "caught"; }

        try { Encoding.GetEncoding(""); }
        catch (ArgumentException) { r3 = "caught"; }

        Console.WriteLine("unknown " + r1 + " " + r2 + " " + r3);

        // With fallbacks: a new encoding, the one of the property is not changed.
        Encoding strict = Encoding.GetEncoding("ascii", EncoderFallback.ExceptionFallback, DecoderFallback.ExceptionFallback);
        Boolean e1 = false, d1 = false;
        try { strict.GetBytes("é"); }
        catch (EncoderFallbackException) { e1 = true; }

        try { strict.GetString(new UInt8[] { 0xE9 }); }
        catch (DecoderFallbackException) { d1 = true; }

        Console.WriteLine("fallbacks " + e1 + " " + d1 + " " + (Encoding.GetEncoding(20127, EncoderFallback.ReplacementFallback, DecoderFallback.ReplacementFallback).CodePage == 20127)
            + " | " + Hex(Encoding.ASCII.GetBytes("é")) + " " + Hex(Encoding.Latin1.GetBytes("é")));

        Console.WriteLine("gb " + Same(Encoding.GetEncoding("gb18030"), Encoding.GB18030) + " " + Same(Encoding.GetEncoding("GB-18030"), Encoding.GB18030)
            + " " + Same(Encoding.GetEncoding(54936), Encoding.GB18030) + " " + Encoding.GB18030.CodePage + " " + Encoding.GB18030.WebName);

        Console.WriteLine("default " + Same(Encoding.Default, Encoding.UTF8));

        UInt8[] utf8 = Encoding.UTF8.GetBytes("aé");
        Console.WriteLine("convert " + Hex(Encoding.Convert(Encoding.Latin1, Encoding.UTF8, new UInt8[] { 0x61 }))
            + " | " + Hex(Encoding.Convert(Encoding.Latin1, Encoding.UTF8, new UInt8[] { 0xE9 }))
            + " | " + Hex(Encoding.Convert(Encoding.UTF8, Encoding.Latin1, utf8, 1, 2))
            + " | " + Hex(Encoding.Convert(Encoding.UTF8, Encoding.Unicode, Encoding.UTF8.GetBytes("é")))
            + " | " + Hex(Encoding.Convert(Encoding.UTF8, Encoding.ASCII, utf8)));

        String r4 = "no";
        try { Encoding.Convert(Encoding.UTF8, Encoding.UTF8, utf8, 2, 5); }
        catch (OutOfRangeException) { r4 = "caught"; }

        Console.WriteLine("range " + r4);

        // GetEncoding gives the encoding that is shared: a fallback is not set on it.
        Console.WriteLine("kept " + (Encoding.GetEncoding("ascii").EncoderFallback is EncoderReplacementFallback)
            + " " + (Encoding.GetEncoding("ascii").EncoderFallback is EncoderExceptionFallback));
    }
}
