// Regex: RegexOptions.ECMAScript: \w \d \s and the word boundary are the characters of ASCII, a reference to a group that did
// not match matches nothing (and the group that refers to itself is empty), \N is a reference only when the group exists,
// else it is an octal character, \p is not there, and the options that can go with it are the case, the lines and Compiled.

/* EXPECT

\w+ | h<00E9>llo w<00F6>rld_1 | ecma 4: h llo w rld_1 | net 2: h<00E9>llo w<00F6>rld_1
\W+ | ab<00E9>!c | ecma 1: <00E9>! | net 1: !
\d+ | 12<0663>3<FF15> | ecma 2: 12 3 | net 1: 12<0663>3<FF15>
\D+ | 1a<0663>2 | ecma 1: a<0663> | net 1: a
\s+ | a b<00A0>c<0009> | ecma 2:   <0009> | net 3:   <00A0> <0009>
\S+ | a b<00A0>c | ecma 2: a b<00A0>c | net 3: a b c
[\w-]+ | ab-<00E9>c | ecma 2: ab- c | net 1: ab-<00E9>c
[^\w]+ | ab<00E9>!c | ecma 1: <00E9>! | net 1: !
[\d\s]+ | 1 2<0663>3 | ecma 2: 1 2 3 | net 1: 1 2<0663>3
[\W\d]+ | a1<00E9>!b | ecma 1: 1<00E9>! | net 2: 1 !
\w\W\w | a<00E9>b | ecma 1: a<00E9>b | net 0:
\bx | <00E9>x | ecma 1: x | net 0:
x\b | x<00E9> | ecma 1: x | net 0:
\Bx | <00E9>x | ecma 0: | net 1: x
\b\w+\b | caf<00E9> ok | ecma 2: caf ok | net 2: caf<00E9> ok
(a)?b\1 | b | ecma 1: b | net 0:
(a)?b\1 | bab | ecma 2: b b | net 0:
(a\1) | aa | ecma 2: a a | net 0:
(a)|b\1 | b | ecma 1: b | net 0:
(?<n>a)?b\k<n> | b | ecma 1: b | net 0:
(a)?(?<=\1)b | b | ecma 1: b | net 0:
\1 | a<0001> | ecma 1: <0001> | net error
\012 | a<000A>b | ecma 1: <000A> | net 1: <000A>
\101 | xAy | ecma 1: A | net 1: A
\8 | a8 | ecma 1: 8 | net error
(a)\1 | aa | ecma 1: aa | net 1: aa
(a)\2 | a<0002> | ecma 1: a<0002> | net error
(a)(b)\2\1 | abba | ecma 1: abba | net 1: abba
(a)\10 | a<0008> | ecma 0: | net 1: a<0008>
(?i)[a-z]+ | ABC<00E9> | ecma 1: ABC | net 1: ABC
^\w+$ | ab<000A>cd | ecma 0: | net 0:
lines 2: ab cd 1: ABC 1: a
(?<=a)b | ab | ecma 1: b | net 1: b
(?>a+)b | aab | ecma 1: aab | net 1: aab
a{2,3} | aaaa | ecma 1: aaa | net 1: aaa
\x41\u0042 | AB | ecma 1: AB | net 1: AB
valid
valid
valid
valid
out of range
out of range
out of range
out of range
out of range
property True
property in class True
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
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

    static String All(String pattern, String text, RegexOptions options)
    {
        try
        {
            MatchCollection ms = Regex.Matches(text, pattern, options);
            String s = ms.Count + ":";
            for (Int32 i = 0; i < ms.Count; i++)
                s += " " + Show(ms[i].Value);

            return s;
        }
        catch (RegexParseException)
        {
            return "error";
        }
    }

    static void T(String pattern, String text)
    {
        Console.WriteLine(pattern + " | " + Show(text) + " | ecma " + All(pattern, text, RegexOptions.ECMAScript) + " | net " + All(pattern, text, RegexOptions.None));
    }

    static String Chars(params Int32[] codes)
    {
        Char[] chars = new Char[codes.Length];
        for (Int32 i = 0; i < codes.Length; i++)
            chars[i] = (Char)codes[i];

        return String.FromChars(chars);
    }

    static void Bad(RegexOptions options)
    {
        try
        {
            new Regex("a", options);
            Console.WriteLine("valid");
        }
        catch (OutOfRangeException)
        {
            Console.WriteLine("out of range");
        }
    }

    [EntryPoint]
    public static void Main()
    {
        // The classes are of ASCII.
        T("\\w+", "h" + Chars(0xE9) + "llo w" + Chars(0xF6) + "rld_1");
        T("\\W+", "ab" + Chars(0xE9) + "!c");
        T("\\d+", "12" + Chars(0x663) + "3" + Chars(0xFF15));
        T("\\D+", "1a" + Chars(0x663) + "2");
        T("\\s+", "a b" + Chars(0xA0) + "c" + Chars(9));
        T("\\S+", "a b" + Chars(0xA0) + "c");
        T("[\\w-]+", "ab-" + Chars(0xE9) + "c");
        T("[^\\w]+", "ab" + Chars(0xE9) + "!c");
        T("[\\d\\s]+", "1 2" + Chars(0x663) + "3");
        T("[\\W\\d]+", "a1" + Chars(0xE9) + "!b");
        T("\\w\\W\\w", "a" + Chars(0xE9) + "b");

        // The boundary of a word.
        T("\\bx", Chars(0xE9) + "x");
        T("x\\b", "x" + Chars(0xE9));
        T("\\Bx", Chars(0xE9) + "x");
        T("\\b\\w+\\b", "caf" + Chars(0xE9) + " ok");

        // A reference to a group that did not match matches nothing; a group that refers to itself.
        T("(a)?b\\1", "b");
        T("(a)?b\\1", "bab");
        T("(a\\1)", "aa");
        T("(a)|b\\1", "b");
        T("(?<n>a)?b\\k<n>", "b");
        T("(a)?(?<=\\1)b", "b");

        // \N is a reference when the group exists, else it is an octal character (8 and 9 are themselves).
        T("\\1", "a" + Chars(1));
        T("\\012", "a" + Chars(10) + "b");
        T("\\101", "xAy");
        T("\\8", "a8");
        T("(a)\\1", "aa");
        T("(a)\\2", "a" + Chars(2));
        T("(a)(b)\\2\\1", "abba");
        T("(a)\\10", "a" + Chars(8));

        // The case, the lines, and the others that are the same.
        T("(?i)[a-z]+", "ABC" + Chars(0xE9));
        T("^\\w+$", "ab\ncd");
        Console.WriteLine("lines " + All("^\\w+$", "ab\ncd", RegexOptions.ECMAScript | RegexOptions.Multiline) + " " + All("[a-z]+", "ABC", RegexOptions.ECMAScript | RegexOptions.IgnoreCase) + " " + All("a", "a", RegexOptions.ECMAScript | RegexOptions.Compiled));
        T("(?<=a)b", "ab");
        T("(?>a+)b", "aab");
        T("a{2,3}", "aaaa");
        T("\\x41\\u0042", "AB");

        // The options that can go with it.
        Bad(RegexOptions.ECMAScript);
        Bad(RegexOptions.ECMAScript | RegexOptions.IgnoreCase);
        Bad(RegexOptions.ECMAScript | RegexOptions.Multiline | RegexOptions.IgnoreCase);
        Bad(RegexOptions.ECMAScript | RegexOptions.Compiled);
        Bad(RegexOptions.ECMAScript | RegexOptions.Singleline);
        Bad(RegexOptions.ECMAScript | RegexOptions.RightToLeft);
        Bad(RegexOptions.ECMAScript | RegexOptions.ExplicitCapture);
        Bad(RegexOptions.ECMAScript | RegexOptions.IgnorePatternWhitespace);
        Bad(RegexOptions.ECMAScript | RegexOptions.CultureInvariant);

        // The properties of Unicode are not in it.
        try
        {
            new Regex("\\p{L}", RegexOptions.ECMAScript);
            Console.WriteLine("property valid");
        }
        catch (RegexParseException e)
        {
            Console.WriteLine("property " + e.Message.Contains("ECMAScript"));
        }

        try
        {
            new Regex("[\\p{Lu}]", RegexOptions.ECMAScript);
            Console.WriteLine("property valid");
        }
        catch (RegexParseException e)
        {
            Console.WriteLine("property in class " + e.Message.Contains("ECMAScript"));
        }

        Console.WriteLine("done");
    }
}
