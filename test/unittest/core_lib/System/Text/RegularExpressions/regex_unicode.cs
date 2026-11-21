// Regex: \p{..} and \P{..} (the categories of Unicode and the blocks), and \w \W \d \D with the definitions of Unicode.

/* EXPECT

\p{Lu} | aB1 _-(<4E2D><00E9>9 | 1: B
\p{Ll} | aB1 _-(<4E2D><00E9>9 | 2: a <00E9>
\p{L} | aB1 _-(<4E2D><00E9>9 | 4: a B <4E2D> <00E9>
\p{Lo} | aB1 _-(<4E2D><00E9>9 | 1: <4E2D>
\p{N} | aB1 _-(<4E2D><00E9>9 | 2: 1 9
\p{Nd} | aB1 _-(<4E2D><00E9>9 | 2: 1 9
\p{P} | aB1 _-(<4E2D><00E9>9 | 3: _ - (
\p{Pc} | aB1 _-(<4E2D><00E9>9 | 1: _
\p{Pd} | aB1 _-(<4E2D><00E9>9 | 1: -
\p{Ps} | aB1 _-(<4E2D><00E9>9 | 1: (
\p{Z} | aB1 _-(<4E2D><00E9>9 | 1:  
\p{Zs} | aB1 _-(<4E2D><00E9>9 | 1:  
\p{S} | a+b$c^d<00A9>e | 4: + $ ^ <00A9>
\p{Sm}\p{Sc} | 1+$2 | 1: +$
\p{C} | a<0009>b<00AD><E000> | 3: <0009> <00AD> <E000>
\p{Cc} | a<0009>b<000A> | 2: <0009> <000A>
\p{M} | e<0301>x | 1: <0301>
\p{Mn} | e<0301>x | 1: <0301>
\p{Lt} | <01C5>a | 1: <01C5>
\p{Cn} | a<0378>b | 1: <0378>
\p{Cs} | a<D800>b | 1: <D800>
\P{L} | aB1 _-(<4E2D><00E9>9 | 6: 1   _ - ( 9
\P{Lu}+ | abCDef | 2: ab ef
[\p{L}\p{N}]+ | aB1 _-(<4E2D><00E9>9 | 2: aB1 <4E2D><00E9>9
[^\p{L}]+ | aB1 _-(<4E2D><00E9>9 | 2: 1 _-( 9
[\P{L}]+ | aB1 _-(<4E2D><00E9>9 | 2: 1 _-( 9
[\p{Lu}-[A]]+ | ABCa | 1: BC
[a-c\p{Nd}]+ | abc123xyz | 1: abc123
\p{Lu}\p{Ll}+ | Hello World | 2: Hello World
^\p{L}+$ | h<00E9>llo | 1: h<00E9>llo
^\p{L}+$ | h<00E9>llo1 | 0:
\p{Nd}{4} | year 2026! | 1: 2026
(?<=\p{Lu})\p{Ll} | aBcD | 1: c
\p{L}+(?=\p{Z}) | ab cd | 1: ab
\p{L}*?\p{N} | abc123 | 3: abc1 2 3
\p{Lu} |  | 0:
\p{IsBasicLatin}+ | ab<4E2D>cd | 2: ab cd
\p{IsCJKUnifiedIdeographs}+ | ab<4E2D><6587>cd | 1: <4E2D><6587>
\P{IsCJKUnifiedIdeographs}+ | ab<4E2D><6587>cd | 2: ab cd
\p{IsGreek}+ | abc<03B1><03B2><03B3>def | 1: <03B1><03B2><03B3>
\p{IsCyrillic}+ | ab<043F><0440><0438>cd | 1: <043F><0440><0438>
\p{IsHiragana}\p{IsKatakana} | <3042><30A2> | 1: <3042><30A2>
[\p{IsLatin-1Supplement}]+ | a<00E9><00FF>z | 1: <00E9><00FF>
\p{IsLatinExtended-A} | a<0100><017F><0180> | 2: <0100> <017F>
\p{IsGeneralPunctuation} | a<2014>b | 1: <2014>
\p{IsHangulSyllables}+ | a<AC00><D7A3>b | 1: <AC00><D7A3>
\p{IsPrivateUse} | a<E000>b | 1: <E000>
\p{IsHalfwidthandFullwidthForms}+ | a<FF21><FF22>b | 1: <FF21><FF22>
\w+ | abc_1 <00E9><4E2D><0301> x-y | 4: abc_1 <00E9><4E2D><0301> x y
\W+ | ab, <00E9>! | 2: ,  !
\d+ | 12<0663><0664>x<FF15>y | 2: 12<0663><0664> <FF15>
\D+ | a1b | 2: a b
\b\w+\b | <00E9> <00E0> | 2: <00E9> <00E0>
\w | <2160><00B2><203F> | 1: <203F>
[\w-]+ | ab-<00E9> | 1: ab-<00E9>
\s+ | a b<00A0>c | 2:   <00A0>
(?i)\p{Lu} | aB | 1: B
(?i)[a-z]+ | ABC | 1: ABC
(?i)\p{IsGreek} | <03B1><03A9> | 2: <03B1> <03A9>
rtl cd 2
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    static String All(String pattern, String text)
    {
        MatchCollection ms = Regex.Matches(text, pattern);
        String s = ms.Count + ":";
        for (Int32 i = 0; i < ms.Count; i++)
            s += " " + Show(ms[i].Value);

        return s;
    }

    // The characters that are not the printable ones of ASCII are written as <XXXX>.
    static String Show(String text)
    {
        String s = "";
        for (Int32 i = 0; i < text.Length; i++)
        {
            Int32 c = (Int32)text[i];
            s += c < 32 || c > 126 ? "<" + c.ToString("X4") + ">" : text[i].ToString();
        }

        return s;
    }

    static void T(String pattern, String text)
    {
        Console.WriteLine(pattern + " | " + Show(text) + " | " + All(pattern, text));
    }

    static String Chars(params Int32[] codes)
    {
        Char[] chars = new Char[codes.Length];
        for (Int32 i = 0; i < codes.Length; i++)
            chars[i] = (Char)codes[i];

        return String.FromChars(chars);
    }

    [EntryPoint]
    public static void Main()
    {
        String mixed = "aB1 _-(中é9";

        // The categories of two letters, and the groups of one letter.
        T("\\p{Lu}", mixed);
        T("\\p{Ll}", mixed);
        T("\\p{L}", mixed);
        T("\\p{Lo}", mixed);
        T("\\p{N}", mixed);
        T("\\p{Nd}", mixed);
        T("\\p{P}", mixed);
        T("\\p{Pc}", mixed);
        T("\\p{Pd}", mixed);
        T("\\p{Ps}", mixed);
        T("\\p{Z}", mixed);
        T("\\p{Zs}", mixed);
        T("\\p{S}", "a+b$c^d©e");
        T("\\p{Sm}\\p{Sc}", "1+$2");
        T("\\p{C}", "a\tb" + Chars(0xAD, 0xE000));
        T("\\p{Cc}", "a\tb\n");
        T("\\p{M}", "e" + Chars(0x301) + "x");
        T("\\p{Mn}", "e" + Chars(0x301) + "x");
        T("\\p{Lt}", Chars(0x1C5) + "a");
        T("\\p{Cn}", "a" + Chars(0x378) + "b");
        T("\\p{Cs}", "a" + Chars(0xD800) + "b");

        // The others: a category that is not the one, in a class, with other parts.
        T("\\P{L}", mixed);
        T("\\P{Lu}+", "abCDef");
        T("[\\p{L}\\p{N}]+", mixed);
        T("[^\\p{L}]+", mixed);
        T("[\\P{L}]+", mixed);
        T("[\\p{Lu}-[A]]+", "ABCa");
        T("[a-c\\p{Nd}]+", "abc123xyz");
        T("\\p{Lu}\\p{Ll}+", "Hello World");
        T("^\\p{L}+$", "héllo");
        T("^\\p{L}+$", "héllo1");
        T("\\p{Nd}{4}", "year 2026!");
        T("(?<=\\p{Lu})\\p{Ll}", "aBcD");
        T("\\p{L}+(?=\\p{Z})", "ab cd");
        T("\\p{L}*?\\p{N}", "abc123");
        T("\\p{Lu}", "");

        // The blocks.
        T("\\p{IsBasicLatin}+", "ab中cd");
        T("\\p{IsCJKUnifiedIdeographs}+", "ab中文cd");
        T("\\P{IsCJKUnifiedIdeographs}+", "ab中文cd");
        T("\\p{IsGreek}+", "abc" + Chars(0x3B1, 0x3B2, 0x3B3) + "def");
        T("\\p{IsCyrillic}+", "ab" + Chars(0x43F, 0x440, 0x438) + "cd");
        T("\\p{IsHiragana}\\p{IsKatakana}", Chars(0x3042, 0x30A2));
        T("[\\p{IsLatin-1Supplement}]+", "aé" + Chars(0xFF) + "z");
        T("\\p{IsLatinExtended-A}", "a" + Chars(0x100, 0x17F, 0x180));
        T("\\p{IsGeneralPunctuation}", "a" + Chars(0x2014) + "b");
        T("\\p{IsHangulSyllables}+", "a" + Chars(0xAC00, 0xD7A3) + "b");
        T("\\p{IsPrivateUse}", "a" + Chars(0xE000) + "b");
        T("\\p{IsHalfwidthandFullwidthForms}+", "a" + Chars(0xFF21, 0xFF22) + "b");

        // \w and \d are the definitions of Unicode: a letter, a mark, a digit, a connector punctuation.
        T("\\w+", "abc_1 é中" + Chars(0x301) + " x-y");
        T("\\W+", "ab, é!");
        T("\\d+", "12" + Chars(0x663, 0x664) + "x" + Chars(0xFF15) + "y");
        T("\\D+", "a1b");
        T("\\b\\w+\\b", "é à");
        T("\\w", Chars(0x2160, 0xB2, 0x203F));
        T("[\\w-]+", "ab-é");
        T("\\s+", "a b" + Chars(0xA0) + "c");

        // The case of the pattern: the classes of the categories are not changed by it.
        T("(?i)\\p{Lu}", "aB");
        T("(?i)[a-z]+", "ABC");
        T("(?i)\\p{IsGreek}", Chars(0x3B1, 0x3A9));

        // The categories and the options, in the right to left direction too.
        Console.WriteLine("rtl " + Regex.Match("ab12cd", "\\p{L}+", RegexOptions.RightToLeft).Value + " " + Regex.Match("ab12cd", "\\p{Nd}", RegexOptions.RightToLeft).Value);

        Console.WriteLine("done");
    }
}
