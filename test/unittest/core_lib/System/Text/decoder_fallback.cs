// DecoderFallback: the replacement and the exception for the bytes that an encoding cannot decode.

/* EXPECT

same 0 0 0 0 0 0
replace [?]a[?][?] | a[?]b
exception 1 2 e2 82 | 1 1 ff | True | True
surrogate 0 1 | 3
utf16 2 1 | 1
utf32 0 4 | 4 4 | 4 3 62
ascii 2 1 80 | 61 62 | True
custom a{1}{3}
kept True True
original 61 ef bf bd | False True
stream a€ | 1 2
bad empty

*/

import System;

using System;
using System.Text;

// A fallback that tells the place of the bytes.
class Place : DecoderFallback
{
    public override String GetReplacement(UInt8[] bytesUnknown, Int32 index) => "{" + index + "}";
}

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

    // Bytes that are hard for the decoders: leads, continuations, the ones that are too long, surrogates, the end.
    static UInt8[] Sample(Int32 seed, Int32 length)
    {
        Int32[] pool = new Int32[] { 0x41, 0x80, 0xC0, 0xC2, 0xE0, 0xED, 0xF0, 0xF4, 0xF5, 0xFF, 0xA9, 0x9F, 0xBF, 0x00, 0xD8, 0xDC };
        UInt8[] bytes = new UInt8[length];

        for (Int32 k = 0; k < length; k++)
        {
            seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
            bytes[k] = (UInt8)pool[(seed >> 8) % pool.Length];
        }

        return bytes;
    }

    // How many samples give a text that is not the one of the encoding without a fallback.
    static Int32 Differences(Encoding plain)
    {
        Encoding fallback = plain.WithDecoderFallback(DecoderFallback.ReplacementFallback);
        Encoding quoted = plain.WithDecoderFallback(new DecoderReplacementFallback(plain.Name == "us-ascii" ? "?" : "�"));
        Int32 count = 0;

        for (Int32 k = 0; k < 300; k++)
        {
            UInt8[] bytes = Sample(k + 1, k % 13);
            String expected = plain.GetString(bytes);

            // The fallback of the library is U+FFFD; the one of the single byte encodings is '?'.
            if (plain.Name == "us-ascii" || plain.Name == "iso-8859-1")
            {
                if (quoted.GetString(bytes) != expected)
                    count++;
            }
            else if (fallback.GetString(bytes) != expected)
            {
                count++;
            }
        }

        return count;
    }

    [EntryPoint]
    public static void Main()
    {
        // The replacement that is given is the one that the encoding has: the texts are the same.
        Console.WriteLine("same " + Differences(Encoding.UTF8) + " " + Differences(Encoding.Unicode) + " "
            + Differences(Encoding.BigEndianUnicode) + " " + Differences(Encoding.UTF32) + " " + Differences(Encoding.ASCII)
            + " " + Differences(Encoding.Latin1));

        Encoding bracket = Encoding.UTF8.WithDecoderFallback(new DecoderReplacementFallback("[?]"));
        Console.WriteLine("replace " + bracket.GetString(new UInt8[] { 0xFF, 0x61, 0xC0, 0x80 }) + " | "
            + bracket.GetString(new UInt8[] { 0x61, 0xFF, 0x62 }));

        // The exception tells the bytes and the place.
        Encoding strict = Encoding.UTF8.WithDecoderFallback(DecoderFallback.ExceptionFallback);
        String info = "";
        try { strict.GetString(new UInt8[] { 0x61, 0xE2, 0x82 }); }
        catch (DecoderFallbackException e) { info = e.Index + " " + e.BytesUnknown.Length + " " + Hex(e.BytesUnknown); }

        String info2 = "";
        try { strict.GetString(new UInt8[] { 0x61, 0xFF, 0x62 }); }
        catch (DecoderFallbackException e) { info2 = e.Index + " " + e.BytesUnknown.Length + " " + Hex(e.BytesUnknown).Substring(0, 2); }

        String valid = strict.GetString(Encoding.UTF8.GetBytes("a€😀é"));
        Boolean caught = false;
        try { strict.GetString(new UInt8[] { 0x80 }, 0, 1); }
        catch (ArgumentException) { caught = true; }

        Console.WriteLine("exception " + info + " | " + info2 + " | " + (valid == "a€😀é") + " | " + caught);

        // A surrogate is three errors, each byte of its own.
        String s3 = "";
        Int32 n3 = 0;
        Encoding counter = Encoding.UTF8.WithDecoderFallback(new Place());
        s3 = counter.GetString(new UInt8[] { 0xED, 0xA0, 0x80 });
        for (Int32 k = 0; k < s3.Length; k++)
        {
            if (s3[k] == '{')
                n3++;
        }

        try { strict.GetString(new UInt8[] { 0xED, 0xA0, 0x80 }); }
        catch (DecoderFallbackException e) { s3 = e.Index + " " + e.BytesUnknown.Length; }

        Console.WriteLine("surrogate " + s3 + " | " + n3);

        // UTF-16: the byte at the end; UTF-32: a value that is not a character, and the bytes at the end.
        Encoding u16 = Encoding.Unicode.WithDecoderFallback(DecoderFallback.ExceptionFallback);
        String r16 = "", r16b = "";
        try { u16.GetString(new UInt8[] { 0x61, 0x00, 0xAA }); }
        catch (DecoderFallbackException e) { r16 = e.Index + " " + e.BytesUnknown.Length; }

        try { r16b = "" + u16.GetString(new UInt8[] { 0x61, 0x00 }).Length; }
        catch (DecoderFallbackException) { r16b = "bad"; }

        Console.WriteLine("utf16 " + r16 + " | " + r16b);

        Encoding u32 = Encoding.UTF32.WithDecoderFallback(DecoderFallback.ExceptionFallback);
        String a32 = "", b32 = "", c32 = "";
        try { u32.GetString(new UInt8[] { 0x00, 0xD8, 0x00, 0x00 }); }
        catch (DecoderFallbackException e) { a32 = e.Index + " " + e.BytesUnknown.Length; }

        try { u32.GetString(new UInt8[] { 0x61, 0, 0, 0, 0x62, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 }); }
        catch (DecoderFallbackException e) { b32 = e.Index + " " + e.BytesUnknown.Length; }

        try { u32.GetString(new UInt8[] { 0x61, 0, 0, 0, 0x62, 0x63, 0x64 }); }
        catch (DecoderFallbackException e) { c32 = e.Index + " " + e.BytesUnknown.Length + " " + Hex(e.BytesUnknown).Substring(0, 2); }

        Console.WriteLine("utf32 " + a32 + " | " + b32 + " | " + c32);

        // ASCII: a byte over 127; Latin1 has no byte that is not a character.
        Encoding ascii = Encoding.ASCII.WithDecoderFallback(DecoderFallback.ExceptionFallback);
        String x = "";
        try { ascii.GetString(new UInt8[] { 0x61, 0x62, 0x80 }); }
        catch (DecoderFallbackException e) { x = e.Index + " " + e.BytesUnknown.Length + " " + Hex(e.BytesUnknown); }

        Encoding latin = Encoding.Latin1.WithDecoderFallback(DecoderFallback.ExceptionFallback);
        Console.WriteLine("ascii " + x + " | " + Hex(ascii.GetBytes(ascii.GetString(new UInt8[] { 0x61, 0x62 }))) + " | "
            + (latin.GetString(new UInt8[] { 0xFF, 0x80 }).Length == 2));

        Console.WriteLine("custom " + Encoding.ASCII.WithDecoderFallback(new Place()).GetString(new UInt8[] { 0x61, 0x80, 0x62, 0xFF }).Replace("b", ""));

        // WithDecoderFallback keeps the fallback of the encoder, and the other way.
        Encoding both = Encoding.ASCII.WithEncoderFallback(EncoderFallback.ExceptionFallback).WithDecoderFallback(DecoderFallback.ExceptionFallback);
        Console.WriteLine("kept " + (both.EncoderFallback is EncoderExceptionFallback) + " "
            + (both.DecoderFallback is DecoderExceptionFallback));

        // The encoding that is made from is not changed.
        Console.WriteLine("original " + Hex(Encoding.UTF8.GetBytes("a")) + " "
            + Hex(Encoding.UTF8.GetBytes(Encoding.UTF8.GetString(new UInt8[] { 0xFF }))) + " | "
            + (Encoding.UTF8.DecoderFallback is DecoderExceptionFallback) + " " + (Encoding.UTF8.DecoderFallback is DecoderReplacementFallback));

        // A decoder in parts: the bytes of a character that is cut wait, and an error is raised when they are left.
        Decoder d = strict.GetDecoder();
        UInt8[] euro = new UInt8[] { 0x61, 0xE2, 0x82, 0xAC };
        String p1 = d.GetString(euro, 0, 2, false);
        String p2 = d.GetString(euro, 2, 2, true);
        String late = "";
        try { strict.GetDecoder().GetString(euro, 0, 3, true); }
        catch (DecoderFallbackException e) { late = e.Index + " " + e.BytesUnknown.Length; }

        Console.WriteLine("stream " + p1 + p2 + " | " + late);

        Boolean empty = false;
        try { strict.GetString(new UInt8[0]); empty = true; }
        catch (DecoderFallbackException) { empty = false; }

        Console.WriteLine("bad " + (empty ? "empty" : "no"));
    }
}
