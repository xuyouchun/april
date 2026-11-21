// CharUnicodeInfo, Char.GetUnicodeCategory, Rune.GetUnicodeCategory: the general category of a character of Unicode, from a
// table that is a resource of the library (the characters of ASCII, the basic plane, the other planes, a text with a pair of
// surrogates).

/* EXPECT

Lu 0
Ll 1
Lt 2
Lm 3
Lo 4
Mn 5
Mc 6
Me 7
Nd 8
Nl 9
No 10
Zs 11
Zl 12
Zp 13
Cc 14
Cf 15
Cs 16
Co 17
Pc 18
Pd 19
Ps 20
Pe 21
Pi 22
Pf 23
Po 24
Sm 25
Sc 26
Sk 27
So 28
Cn 29
first 14
del 14
nbsp 11
fe 29
ffff 29
deseret 0
emoji 28
digit 8
tag 15
plane15 17
last 29
over 29
negative 29
arabic digit 8
devanagari digit 8
fullwidth digit 8
greek 0
cyrillic 1
hiragana 4
hangul 4
fullwidth A 0
char 0 4 11 8
rune 28 1 0
text 1 28 16 8
lone 16 16
index out of range
counts 2048 6400 65 17 370 48973 1432
done

*/

import System;

using System;
using System.Globalization;
using System.Text;

class Program
{
    static void Show(String label, Int32 codePoint)
    {
        Console.WriteLine(label + " " + (Int32)CharUnicodeInfo.GetUnicodeCategory(codePoint));
    }

    static void Show(String label, Char c)
    {
        Show(label, (Int32)c);
    }

    [EntryPoint]
    public static void Main()
    {
        // One character of each category, by the number of the category (the numbers of .NET).
        Show("Lu", 'A');
        Show("Ll", 'a');
        Show("Lt", 0x01C5);
        Show("Lm", 0x02B0);
        Show("Lo", 0x4E2D);
        Show("Mn", 0x0301);
        Show("Mc", 0x0903);
        Show("Me", 0x20DD);
        Show("Nd", '5');
        Show("Nl", 0x2167);
        Show("No", 0x00B2);
        Show("Zs", ' ');
        Show("Zl", 0x2028);
        Show("Zp", 0x2029);
        Show("Cc", '\n');
        Show("Cf", 0x00AD);
        Show("Cs", 0xD800);
        Show("Co", 0xE000);
        Show("Pc", '_');
        Show("Pd", '-');
        Show("Ps", '(');
        Show("Pe", ')');
        Show("Pi", 0x00AB);
        Show("Pf", 0x00BB);
        Show("Po", '!');
        Show("Sm", '+');
        Show("Sc", '$');
        Show("Sk", '^');
        Show("So", 0x00A9);
        Show("Cn", 0x0378);

        // The boundaries of the table, and the other planes.
        Show("first", 0);
        Show("del", 0x7F);
        Show("nbsp", 0xA0);
        Show("fe", 0xFFFE);
        Show("ffff", 0xFFFF);
        Show("deseret", 0x10400);
        Show("emoji", 0x1F600);
        Show("digit", 0x1D7CE);
        Show("tag", 0xE0001);
        Show("plane15", 0xF0000);
        Show("last", 0x10FFFF);
        Show("over", 0x110000);
        Show("negative", -1);

        // Digits and letters of other scripts.
        Show("arabic digit", 0x0663);
        Show("devanagari digit", 0x0966);
        Show("fullwidth digit", 0xFF15);
        Show("greek", 0x03A9);
        Show("cyrillic", 0x0436);
        Show("hiragana", 0x3042);
        Show("hangul", 0xAC00);
        Show("fullwidth A", 0xFF21);

        // The methods of Char, of Rune and of a text.
        Console.WriteLine("char " + (Int32)Char.GetUnicodeCategory('A') + " " + (Int32)Char.GetUnicodeCategory('中') + " " + (Int32)Char.GetUnicodeCategory(' ') + " " + (Int32)CharUnicodeInfo.GetUnicodeCategory('9'));
        Console.WriteLine("rune " + (Int32)Rune.GetUnicodeCategory(new Rune(0x1F600)) + " " + (Int32)Rune.GetUnicodeCategory(new Rune('a')) + " " + (Int32)Rune.GetUnicodeCategory(new Rune(0x10400)));

        String text = "a" + "😀" + "1";
        Console.WriteLine("text " + (Int32)CharUnicodeInfo.GetUnicodeCategory(text, 0) + " " + (Int32)CharUnicodeInfo.GetUnicodeCategory(text, 1) + " " + (Int32)CharUnicodeInfo.GetUnicodeCategory(text, 2) + " " + (Int32)CharUnicodeInfo.GetUnicodeCategory(text, 3));
        Console.WriteLine("lone " + (Int32)CharUnicodeInfo.GetUnicodeCategory(String.FromChars(new Char[] { (Char)0xD83D }), 0) + " " + (Int32)CharUnicodeInfo.GetUnicodeCategory(String.FromChars(new Char[] { (Char)0xDE00, 'x' }), 0));

        try
        {
            CharUnicodeInfo.GetUnicodeCategory("ab", 2);
            Console.WriteLine("no error");
        }
        catch (OutOfRangeException)
        {
            Console.WriteLine("index out of range");
        }

        // How many characters of the basic plane have a category: a count that checks the table as a whole.
        Int32[] counts = new Int32[30];
        for (Int32 c = 0; c < 65536; c++)
            counts[(Int32)CharUnicodeInfo.GetUnicodeCategory((Char)c)]++;

        Int32 letters = counts[0] + counts[1] + counts[2] + counts[3] + counts[4];
        Console.WriteLine("counts " + counts[16] + " " + counts[17] + " " + counts[14] + " " + counts[11] + " " + counts[8] + " " + letters + " " + counts[29]);

        Console.WriteLine("done");
    }
}
