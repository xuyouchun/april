// System.Text.Encoding: UTF-8, UTF-16 (both byte orders), UTF-32, ASCII and Latin1.

/* EXPECT

utf8 41 c3a9 e4b8ad f09f9880 | 13 | 13
utf8 back A é 中 😀 True
bad A,fffd,fffd,fffd,B | fffd,fffd,fffd | fffd,x
lone efbfbd78 4
utf16 4100 2d4e | 0041 4e2d | A中 fffd 2
utf32 41000000 00f60100 | A😀 True fffd
ascii 613f3f62 3f a ?
latin1 61e93f e9 ?
names utf-8 utf-16 utf-16BE utf-32 us-ascii iso-8859-1
max 9 6 6 12 4 4
same True True
chars 3 3 5
range caught caught

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
            sb.Append(digits[b >> 4]).Append(digits[b & 15]);
        }

        return sb.ToString();
    }

    static String Hex16(String s)
    {
        StringBuilder sb = new StringBuilder();
        String digits = "0123456789abcdef";

        for (Int32 k = 0; k < s.Length; k++)
        {
            if (k > 0)
                sb.Append(',');

            Int32 c = (Int32)s[k];
            if (c < 128)
            {
                sb.Append(s[k]);
            }
            else
            {
                sb.Append(digits[c >> 12]).Append(digits[(c >> 8) & 15]).Append(digits[(c >> 4) & 15]).Append(digits[c & 15]);
            }
        }

        return sb.ToString();
    }

    static UInt8[] Bytes(Int32 a, Int32 b, Int32 c, Int32 d, Int32 e)
    {
        UInt8[] bytes = new UInt8[5];
        bytes[0] = (UInt8)a;
        bytes[1] = (UInt8)b;
        bytes[2] = (UInt8)c;
        bytes[3] = (UInt8)d;
        bytes[4] = (UInt8)e;
        return bytes;
    }

    [EntryPoint]
    public static void Main()
    {
        Encoding utf8 = Encoding.UTF8;
        String text = "A é 中 😀";

        Console.WriteLine("utf8 " + Hex(utf8.GetBytes("A")) + " " + Hex(utf8.GetBytes("é")) + " " + Hex(utf8.GetBytes("中"))
            + " " + Hex(utf8.GetBytes("😀")) + " | " + utf8.GetByteCount(text) + " | " + utf8.GetBytes(text).Length);
        Console.WriteLine("utf8 back " + utf8.GetString(utf8.GetBytes(text)) + " " + (utf8.GetString(utf8.GetBytes(text)) == text));

        // Bytes that are not UTF-8: C0 80 is too long, E4 B8 is not complete, ED A0 80 is a surrogate.
        UInt8[] bad = Bytes(0x41, 0xC0, 0x80, 0xE4, 0xB8);
        UInt8[] bad2 = new UInt8[6];
        for (Int32 k = 0; k < 5; k++)
            bad2[k] = bad[k];
        bad2[5] = (UInt8)0x42;
        UInt8[] sur = new UInt8[3];
        sur[0] = (UInt8)0xED;
        sur[1] = (UInt8)0xA0;
        sur[2] = (UInt8)0x80;
        UInt8[] cut = new UInt8[2];
        cut[0] = (UInt8)0xE4;
        cut[1] = (UInt8)0x78;
        Console.WriteLine("bad " + Hex16(utf8.GetString(bad2)) + " | " + Hex16(utf8.GetString(sur)) + " | " + Hex16(utf8.GetString(cut)));

        String lone = String.FromChars(new Char[] { '\uD800', 'x' });
        Console.WriteLine("lone " + Hex(utf8.GetBytes(lone)) + " " + utf8.GetByteCount(lone));

        Encoding le = Encoding.Unicode, be = Encoding.BigEndianUnicode;
        UInt8[] odd = new UInt8[3];
        odd[0] = (UInt8)0x41;
        odd[1] = (UInt8)0x00;
        odd[2] = (UInt8)0x7F;
        Console.WriteLine("utf16 " + Hex(le.GetBytes("A")) + " " + Hex(le.GetBytes("中")) + " | " + Hex(be.GetBytes("A")) + " "
            + Hex(be.GetBytes("中")) + " | " + le.GetString(le.GetBytes("A中")) + " " + Hex16(le.GetString(odd)).Substring(2) + " " + le.GetByteCount("A") );

        Encoding u32 = Encoding.UTF32;
        String pair = "A😀";
        Console.WriteLine("utf32 " + Hex(u32.GetBytes("A")) + " " + Hex(u32.GetBytes("😀")) + " | " + u32.GetString(u32.GetBytes(pair))
            + " " + (u32.GetString(u32.GetBytes(pair)) == pair) + " " + Hex16(u32.GetString(odd)));

        Encoding ascii = Encoding.ASCII, latin1 = Encoding.Latin1;
        UInt8[] high = new UInt8[2];
        high[0] = (UInt8)0x61;
        high[1] = (UInt8)0xE9;
        Console.WriteLine("ascii " + Hex(ascii.GetBytes("aé😀b")) + " " + Hex(ascii.GetBytes("中")) + " " + ascii.GetString(ascii.GetBytes("a")) + " "
            + ascii.GetString(high).Substring(1));
        Console.WriteLine("latin1 " + Hex(latin1.GetBytes("aé中")) + " " + Hex(latin1.GetBytes("é")) + " " + latin1.GetString(latin1.GetBytes("中")));

        Console.WriteLine("names " + utf8.Name + " " + le.Name + " " + be.Name + " " + u32.Name + " " + ascii.Name + " " + latin1.Name);
        Console.WriteLine("max " + utf8.GetMaxByteCount(2) + " " + le.GetMaxByteCount(2) + " " + be.GetMaxByteCount(2) + " "
            + u32.GetMaxByteCount(2) + " " + ascii.GetMaxByteCount(3) + " " + latin1.GetMaxByteCount(3));
        Console.WriteLine("same " + Object.ReferenceEquals(Encoding.UTF8, utf8) + " " + Object.ReferenceEquals(Encoding.Unicode, le));

        Char[] chars = utf8.GetChars(utf8.GetBytes("a中b"));
        Console.WriteLine("chars " + chars.Length + " " + utf8.GetCharCount(utf8.GetBytes("a中b")) + " " + utf8.GetByteCount(chars));

        String caught = "";
        try { utf8.GetBytes("abc", new UInt8[2], 0); } catch (OutOfRangeException) { caught += "range"; }
        try { utf8.GetString(bad2, 4, 5); } catch (OutOfRangeException) { caught += " caught"; }
        try { le.GetBytes("a", new UInt8[2], 1); } catch (OutOfRangeException) { caught += " caught"; }
        Console.WriteLine(caught);
    }
}
