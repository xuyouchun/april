// Rune: a scalar value of any plane; the pair of surrogates, the checks, the order, the text and the conversions.

/* EXPECT

basic 65 True 1 1 1 A
plane 128512 False 2 4 1 😀
check True False False False False
at 128512 65 65533 4 5
runes 3 128512 98 True
order True True False 0 -1 1 True
case 1055 1087 True False True False False
convert 233 é 55357 True
caught 3
builder a😀b 4

*/

import System;

using System;
using System.Text;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Rune a = new Rune('A');
        Console.WriteLine("basic " + a.Value + " " + a.IsBmp + " " + a.Utf16SequenceLength + " " + a.Utf8SequenceLength
            + " " + (a.Plane + 1) + " " + a);

        Rune e = new Rune(0x1F600);
        Rune pair = new Rune('\uD83D', '\uDE00');
        Console.WriteLine("plane " + e.Value + " " + e.IsBmp + " " + e.Utf16SequenceLength + " " + e.Utf8SequenceLength
            + " " + (e == pair ? 1 : 0) + " " + e);

        Console.WriteLine("check " + Rune.IsValid(0x10FFFF) + " " + Rune.IsValid(0x110000) + " " + Rune.IsValid(0xD800)
            + " " + Rune.IsValid(-1) + " " + Rune.IsValid(0xDFFF));

        String s = "A😀\uD800b";
        Console.WriteLine("at " + Rune.GetRuneAt(s, 1).Value + " " + Rune.GetRuneAt(s, 0).Value + " "
            + Rune.GetRuneAt(s, 3).Value + " " + Rune.CountRunes(s) + " " + s.Length);

        Rune[] runes = Rune.ToRunes("a😀b");
        Console.WriteLine("runes " + runes.Length + " " + runes[1].Value + " " + runes[2].Value + " "
            + (Rune.ToRunes("😀")[0] == e));

        Rune b = new Rune('B');
        Console.WriteLine("order " + (a < b) + " " + (b >= a) + " " + (a == b) + " " + a.CompareTo(new Rune('A'))
            + " " + a.CompareTo(e) + " " + e.CompareTo(a) + " " + a.Equals((Object)new Rune('A')));

        Rune cyr = new Rune(0x43F);
        Console.WriteLine("case " + Rune.ToUpper(cyr).Value + " " + Rune.ToLower(new Rune(0x41F)).Value + " "
            + Rune.IsLetter(cyr) + " " + Rune.IsLetter(e) + " " + Rune.IsDigit(new Rune('7')) + " "
            + Rune.IsWhiteSpace(a) + " " + Rune.IsUpper(cyr));

        Rune r = (Rune)0xE9;
        Console.WriteLine("convert " + (Int32)r + " " + r + " " + (Int32)(Rune.GetRuneAt("😀", 0).ToString())[0]
            + " " + ((Rune)'x' == new Rune(0x78)));

        Int32 caught = 0;
        try { Rune bad = new Rune(0xD800); } catch (ArgumentException) { caught++; }
        try { Rune bad = new Rune(0x110000); } catch (ArgumentException) { caught++; }
        try { Rune bad = new Rune('\uDC00', '\uD83D'); } catch (ArgumentException) { caught++; }
        Console.WriteLine("caught " + caught);

        StringBuilder sb = new StringBuilder("a");
        sb.Append(e).Append(new Rune('b'));
        Console.WriteLine("builder " + sb + " " + sb.Length);
    }
}
