// GB18030: the bytes of the characters (the values are those of the gb18030 codec of Python), every character of the
// basic plane, the supplementary planes, the bytes that are not valid, the conversion in parts, the fallbacks.

/* EXPECT

props gb18030 54936 Chinese Simplified (GB18030) False True True 16
single 41 | d6 d0 | c4 e3 ba c3 | a2 e3 | a8 a6 | a1 a1 | aa a1 | a3 e1
four 81 30 81 30 | 84 31 a4 39 | 84 31 a4 37 | 94 39 fc 36 | 90 30 81 30 | e3 32 9a 35 | 81 35 f4 37
mixed 61 d6 d0 62 94 39 fc 36 63 | 9
back True True True True
bmp 63488 205688 237036340 True
supplementary 61681 246724 28998003 True
count 9 12 2 4 caught
invalid 1 1 1 | 1 1 | 2 | True | 4
unmapped 1 1 2
parts True True
fallback 1 1 | 1 1 | caught
errors a� | 61 84 31 a4 37

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

    static Int64 Hash(UInt8[] bytes)
    {
        Int64 h = 0;

        for (Int32 k = 0; k < bytes.Length; k++)
            h = (h * 131 + (Int64)bytes[k]) % 1000000007L;

        return h;
    }

    // The text of the bytes that are given in parts of the size.
    static String Parts(Encoding e, UInt8[] bytes, Int32 size)
    {
        Decoder d = e.GetDecoder();
        String result = "";
        Int32 k = 0;

        while (k < bytes.Length)
        {
            Int32 n = bytes.Length - k < size ? bytes.Length - k : size;
            result += d.GetString(bytes, k, n, k + n >= bytes.Length);
            k += n;
        }

        return result;
    }

    static Boolean AllSizes(Encoding e, String text)
    {
        UInt8[] bytes = e.GetBytes(text);
        String whole = e.GetString(bytes);

        for (Int32 size = 1; size <= bytes.Length + 1; size++)
        {
            if (Parts(e, bytes, size) != whole)
                return false;
        }

        return true;
    }

    static String Decode(Encoding e, UInt8[] bytes) => e.GetString(bytes);

    static String Chars(String s)
    {
        String text = "";

        for (Int32 k = 0; k < s.Length; k++)
            text += (k > 0 ? " " : "") + (Int32)s[k];

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Encoding gb = Encoding.GB18030;

        Console.WriteLine("props " + gb.Name + " " + gb.CodePage + " " + gb.EncodingName + " " + gb.IsSingleByte
            + " " + Object.ReferenceEquals(Encoding.GetEncoding("GB18030"), gb) + " " + Object.ReferenceEquals(Encoding.GetEncoding(54936), gb)
            + " " + gb.GetMaxByteCount(3));

        Console.WriteLine("single " + Hex(gb.GetBytes("A")) + " | " + Hex(gb.GetBytes("\u4E2D")) + " | " + Hex(gb.GetBytes("\u4F60\u597D"))
            + " | " + Hex(gb.GetBytes("\u20AC")) + " | " + Hex(gb.GetBytes("\u00E9")) + " | " + Hex(gb.GetBytes("\u3000"))
            + " | " + Hex(gb.GetBytes("\uE000")) + " | " + Hex(gb.GetBytes("\uFF41")));

        Console.WriteLine("four " + Hex(gb.GetBytes("\u0080")) + " | " + Hex(gb.GetBytes("\uFFFF")) + " | " + Hex(gb.GetBytes("\uFFFD"))
            + " | " + Hex(gb.GetBytes("\U0001F600")) + " | " + Hex(gb.GetBytes("\U00010000")) + " | " + Hex(gb.GetBytes("\U0010FFFF"))
            + " | " + Hex(gb.GetBytes("\u1E3F")));

        String mixed = "a\u4E2Db\U0001F600c";
        Console.WriteLine("mixed " + Hex(gb.GetBytes(mixed)) + " | " + gb.GetBytes(mixed).Length);

        // The bytes are read back.
        String back1 = gb.GetString(gb.GetBytes(mixed));
        String back2 = gb.GetString(new UInt8[] { 0xD6, 0xD0, 0x81, 0x30, 0x81, 0x30, 0x94, 0x39, 0xFC, 0x36 });
        Console.WriteLine("back " + (back1 == mixed) + " " + (back2 == "\u4E2D\u0080\U0001F600") + " "
            + (gb.GetString(new UInt8[] { 0xA2, 0xE3 }) == "\u20AC") + " " + (gb.GetString(new UInt8[] { 0xE3, 0x32, 0x9A, 0x35 }) == "\U0010FFFF"));

        // Every character of the basic plane that is not a surrogate: the bytes are those of Python, and are read back.
        StringBuilder all = new StringBuilder(70000);
        for (Int32 c = 0; c < 0x10000; c++)
        {
            if (c < 0xD800 || c > 0xDFFF)
                all.Append((Char)c);
        }

        String bmp = all.ToString();
        UInt8[] bmpBytes = gb.GetBytes(bmp);
        Console.WriteLine("bmp " + bmp.Length + " " + bmpBytes.Length + " " + Hash(bmpBytes) + " " + (gb.GetString(bmpBytes) == bmp));

        // One character of 17 of the supplementary planes.
        StringBuilder planes = new StringBuilder(130000);
        for (Int32 c = 0x10000; c < 0x110000; c += 17)
            planes.Append(new Rune(c));

        String supplementary = planes.ToString();
        UInt8[] planeBytes = gb.GetBytes(supplementary);
        Console.WriteLine("supplementary " + Rune.CountRunes(supplementary) + " " + planeBytes.Length + " " + Hash(planeBytes)
            + " " + (gb.GetString(planeBytes) == supplementary));

        // The count, and the room that is checked.
        UInt8[] room = new UInt8[7];
        String caught = "no";
        try { gb.GetBytes("\u4E2D\u4E2D\u4E2D\u4E2D", room, 0); }
        catch (OutOfRangeException) { caught = "caught"; }

        Console.WriteLine("count " + gb.GetByteCount(mixed) + " " + gb.GetByteCount("\u4E2D\U0001F600\u0080\u4E2D") + " "
            + gb.GetBytes(new Char[] { 'a', 'b' }).Length + " " + gb.GetBytes("\u4E2D\u4E2D", room, 1) + " " + caught);

        // The bytes that are not valid: an error of one byte (a lead byte that has nothing right), the bytes at the end
        // that are the beginning of a code are one error, a code of four bytes that is not a character is one error.
        Console.WriteLine("invalid " + Decode(gb, new UInt8[] { 0x81 }).Length + " " + Decode(gb, new UInt8[] { 0x80 }).Length
            + " " + Decode(gb, new UInt8[] { 0xFF }).Length + " | " + Decode(gb, new UInt8[] { 0x81, 0x30 }).Length
            + " " + Decode(gb, new UInt8[] { 0x81, 0x30, 0x81 }).Length + " | " + Decode(gb, new UInt8[] { 0x81, 0x20 }).Length
            + " | " + (Decode(gb, new UInt8[] { 0x81, 0x30, 0x20, 0x30 }) == "\uFFFD0 0")
            + " | " + Decode(gb, new UInt8[] { 0x84, 0x31, 0xA4, 0x3A }).Length);

        Console.WriteLine("unmapped " + Decode(gb, new UInt8[] { 0x84, 0x31, 0xA5, 0x30 }).Length + " "
            + Decode(gb, new UInt8[] { 0xE3, 0x32, 0x9A, 0x36 }).Length + " " + Decode(gb, new UInt8[] { 0x84, 0x31, 0xA4, 0x39, 0xFF }).Length);

        // In parts: a code that is cut waits for the next bytes.
        Console.WriteLine("parts " + AllSizes(gb, mixed + "\u20AC\u0080\uFFFF") + " " + AllSizes(gb, bmp.Substring(30000, 600) + supplementary.Substring(0, 400)));

        // The fallbacks.
        Encoding strict = gb.WithEncoderFallback(EncoderFallback.ExceptionFallback).WithDecoderFallback(DecoderFallback.ExceptionFallback);
        String lone = String.FromChars(new Char[] { '\uD83D' });
        String encoderInfo = "no";
        try { strict.GetBytes("a" + lone + "b"); }
        catch (EncoderFallbackException e) { encoderInfo = e.Index + " " + e.UnknownText.Length; }

        String decoderInfo = "no";
        try { strict.GetString(new UInt8[] { 0x61, 0x81, 0x20 }); }
        catch (DecoderFallbackException e) { decoderInfo = e.Index + " " + e.BytesUnknown.Length; }

        String tail = "no";
        try { strict.GetString(new UInt8[] { 0xD6, 0xD0, 0x81, 0x30 }); }
        catch (DecoderFallbackException e) { tail = "caught"; }

        Console.WriteLine("fallback " + encoderInfo + " | " + decoderInfo + " | " + tail);

        // A surrogate that has no pair is U+FFFD.
        Console.WriteLine("errors " + gb.GetString(gb.GetBytes("a" + lone)) + " | " + Hex(gb.GetBytes("a" + lone)));
    }
}
