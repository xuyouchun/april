// EncoderFallback: the replacement and the exception for the characters that an encoding cannot code.

/* EXPECT

default 61 3f 3f | ef bf bd 61
property ? | 65533 | True True
replace 61 5b 3f 5d 5b 3f 5d 62 | 4 | 61 5b 3f 5d
exception 1 é 1 | 2 2 | caught
utf8 61 f0 9f 98 80 | 1 1 | 0 1 | False
utf16 d8 3d | True
latin1 e9 | 1 中 | 3f
custom a{1}{2}bc | 9
original 61 3f
encoder 61 | f0 9f 98 80 | 0 1
badreplacement True
unchanged True False

*/

import System;

using System;
using System.Text;

// A fallback that tells the place of the character that is replaced.
class Place : EncoderFallback
{
    public override String GetReplacement(String unknown, Int32 index) => "{" + index + "}";
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

    [EntryPoint]
    public static void Main()
    {
        // Without a fallback the encodings replace as before.
        String hi = String.FromChars(new Char[] { '\uD83D' });
        Console.WriteLine("default " + Hex(Encoding.ASCII.GetBytes("aé😀")) + " | " + Hex(Encoding.UTF8.GetBytes(hi + "a")));

        Encoding ascii = Encoding.ASCII;
        EncoderReplacementFallback shown = (EncoderReplacementFallback)Encoding.UTF8.EncoderFallback;
        Console.WriteLine("property " + ((EncoderReplacementFallback)ascii.EncoderFallback).DefaultString
            + " | " + (Int32)shown.DefaultString[0] + " | " + (ascii.EncoderFallback is EncoderReplacementFallback)
            + " " + (ascii.WithEncoderFallback(EncoderFallback.ExceptionFallback).EncoderFallback is EncoderExceptionFallback));

        Encoding bracket = ascii.WithEncoderFallback(new EncoderReplacementFallback("[?]"));
        Console.WriteLine("replace " + Hex(bracket.GetBytes("aé😀b")) + " | " + bracket.GetByteCount("a😀") + " | "
            + Hex(bracket.GetBytes("a" + hi).Length == 4 ? bracket.GetBytes("a" + hi) : new UInt8[0]));

        // The exception tells the characters and the place.
        Encoding strict = ascii.WithEncoderFallback(EncoderFallback.ExceptionFallback);
        String info = "";
        try { strict.GetBytes("aé"); }
        catch (EncoderFallbackException e) { info = e.Index + " " + e.UnknownText + " " + e.UnknownText.Length; }

        String info2 = "";
        try { strict.GetByteCount("ab😀"); }
        catch (EncoderFallbackException e) { info2 = e.Index + " " + e.UnknownText.Length; }

        Boolean caught = false;
        try { strict.GetBytes("é", new UInt8[4], 0); }
        catch (ArgumentException) { caught = true; }

        Console.WriteLine("exception " + info + " | " + info2 + " | " + (caught ? "caught" : "no"));

        // UTF-8: a pair is coded, a surrogate that has no pair is not.
        Encoding u8 = Encoding.UTF8.WithEncoderFallback(EncoderFallback.ExceptionFallback);
        String ok = Hex(u8.GetBytes("a😀"));
        String lone = "";
        try { u8.GetBytes("a" + hi + "b"); }
        catch (EncoderFallbackException e) { lone = e.Index + " " + e.UnknownText.Length; }

        String lone2 = "";
        try { u8.GetBytes(hi); }
        catch (EncoderFallbackException e) { lone2 = e.Index + " " + e.UnknownText.Length; }

        Boolean hasBom = Encoding.UTF8.WithEncoderFallback(EncoderFallback.ExceptionFallback).GetPreamble().Length > 0;
        Console.WriteLine("utf8 " + ok + " | " + lone + " | " + lone2 + " | " + hasBom);

        // UTF-16 keeps every unit: nothing is unknown.
        Encoding u16 = Encoding.BigEndianUnicode.WithEncoderFallback(EncoderFallback.ExceptionFallback);
        Console.WriteLine("utf16 " + Hex(u16.GetBytes(hi)).Substring(0, 5) + " | " + (u16.GetBytes(hi).Length == 2));

        Encoding l1 = Encoding.Latin1.WithEncoderFallback(EncoderFallback.ExceptionFallback);
        String l1text = "";
        try { l1.GetBytes("a中"); }
        catch (EncoderFallbackException e) { l1text = e.Index + " " + e.UnknownText; }

        Console.WriteLine("latin1 " + Hex(l1.GetBytes("é")) + " | " + l1text.Substring(0, 1) + " " + l1text.Substring(2) + " | "
            + Hex(Encoding.Latin1.GetBytes("中")));

        Encoding custom = ascii.WithEncoderFallback(new Place());
        Console.WriteLine("custom " + custom.GetString(custom.GetBytes("aéébc")).Replace("é", "") + " | " + custom.GetBytes("aéébc").Length);
        Console.WriteLine("original " + Hex(ascii.GetBytes("a")) + " " + Hex(ascii.GetBytes("é")).Substring(0, 2));

        // The encoder keeps the high surrogate of a part, and a lone one fails at the end.
        Encoder enc = u8.GetEncoder();
        UInt8[] a = enc.GetBytes("a" + hi, false);
        UInt8[] b = enc.GetBytes(String.FromChars(new Char[] { '\uDE00' }), true);
        Boolean failed = false;
        try { u8.GetEncoder().GetBytes(hi, true); }
        catch (EncoderFallbackException) { failed = true; }

        Console.WriteLine("encoder " + Hex(a) + " | " + Hex(b) + " | " + a.Length / 2 + " " + (failed ? 1 : 0));

        Boolean bad = false;
        try { new EncoderReplacementFallback(hi); }
        catch (ArgumentException) { bad = true; }

        Console.WriteLine("badreplacement " + bad);

        Console.WriteLine("unchanged " + (ascii.GetBytes("é").Length == 1) + " " + (ascii.EncoderFallback is EncoderExceptionFallback));
    }
}
