// Encoder, Decoder and GetPreamble: the conversion in parts gives what the conversion of the whole gives.

/* EXPECT

split utf8 True 13 4
split utf16 True True
split utf32 True 1
split single True True
pending 0 0 e2 82 ac 2
flush a� True
encoder [] [f0 9f 98 80] [ef bf bd]
encoder2 [61] [ef bf bd 62] [f0 9f 98 80 63]
count 1 4 1
preamble 0 3 ef bb bf | 2 ff fe | 2 fe ff | 4 ff fe 00 00 | 0
caught True

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

    [EntryPoint]
    public static void Main()
    {
        String text = "a€😀é中";
        Console.WriteLine("split utf8 " + AllSizes(Encoding.UTF8, text) + " " + Encoding.UTF8.GetBytes(text).Length
            + " " + Encoding.UTF8.GetEncoder().GetBytes("x€", true).Length);

        Console.WriteLine("split utf16 " + AllSizes(Encoding.Unicode, text) + " " + AllSizes(Encoding.BigEndianUnicode, text));
        Console.WriteLine("split utf32 " + AllSizes(Encoding.UTF32, text) + " " + Encoding.UTF32.GetBytes("😀").Length / 4);
        Console.WriteLine("split single " + AllSizes(Encoding.Latin1, "aé") + " " + AllSizes(Encoding.ASCII, "ab?"));

        // The first two bytes of a three-byte sequence wait for the third.
        Decoder d = Encoding.UTF8.GetDecoder();
        UInt8[] euro = new UInt8[] { 0xE2, 0x82, 0xAC };
        String first = d.GetString(euro, 0, 2, false);
        Int32 kept = d.PendingByteCount;
        String second = d.GetString(euro, 2, 1, false);
        Console.WriteLine("pending " + first.Length + " " + d.PendingByteCount + " " + Hex(Encoding.UTF8.GetBytes(second)) + " " + kept);

        // flush: the bytes that are left are U+FFFD.
        Decoder f = Encoding.UTF8.GetDecoder();
        String cut = f.GetString(new UInt8[] { 0x61, 0xE2, 0x82 }, true);
        Console.WriteLine("flush " + cut + " " + (cut == "a�"));

        // A string constant cannot have a surrogate that has no pair.
        String hi = String.FromChars(new Char[] { '\uD83D' }), lo = String.FromChars(new Char[] { '\uDE00' });

        // A pair that is cut between two parts is one character.
        Encoder e = Encoding.UTF8.GetEncoder();
        UInt8[] a = e.GetBytes(hi, false);
        UInt8[] b = e.GetBytes(lo, true);
        UInt8[] lone = Encoding.UTF8.GetEncoder().GetBytes(hi, true);
        Console.WriteLine("encoder [" + Hex(a) + "] [" + Hex(b) + "] [" + Hex(lone) + "]");

        Encoder e2 = Encoding.UTF8.GetEncoder();
        UInt8[] p1 = e2.GetBytes("a" + hi, false);
        UInt8[] p2 = e2.GetBytes("b" + hi, false);
        UInt8[] p3 = e2.GetBytes(new Char[] { '\uDE00', 'c' }, 0, 2, true);
        Console.WriteLine("encoder2 [" + Hex(p1) + "] [" + Hex(p2) + "] [" + Hex(p3) + "]");

        // GetByteCount does not change the state.
        Encoder c = Encoding.UTF8.GetEncoder();
        Int32 n1 = c.GetByteCount("a" + hi, false), n2 = c.GetByteCount("a" + hi, true);
        Console.WriteLine("count " + n1 + " " + n2 + " " + c.GetBytes("a", false).Length);

        Encoding u8 = new UTF8Encoding(true), le = new UnicodeEncoding(false, true), be = new UnicodeEncoding(true, true);
        Encoding u32 = new UTF32Encoding(true);
        Console.WriteLine("preamble " + Encoding.UTF8.GetPreamble().Length + " " + u8.GetPreamble().Length
            + " " + Hex(u8.GetPreamble()) + " | " + le.GetPreamble().Length + " " + Hex(le.GetPreamble())
            + " | " + be.GetPreamble().Length + " " + Hex(be.GetPreamble()) + " | " + u32.GetPreamble().Length
            + " " + Hex(u32.GetPreamble()) + " | " + Encoding.ASCII.GetPreamble().Length);

        Boolean caught = false;
        try { Encoding.UTF8.GetDecoder().GetString(new UInt8[2], 1, 5, true); }
        catch (OutOfRangeException) { caught = true; }
        Console.WriteLine("caught " + caught);
    }
}
