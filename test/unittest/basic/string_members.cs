// The members of String and Char: the indexer, searching, trimming, padding, replacing, splitting, the cases.

/* EXPECT

index b 14
empty True True True
indexof 2 -1 4 3
last 5 3 -1
contains True False
starts True False ends True False
trim [x y] [x y  ] [  x y]
pad [   7] [7...]
insert a-bc remove ac
replace a-b-c|abc|x
split 3 a|b|c
split2 3 [ab][][c]
upper ABC-Ü-Ж lower abc-ü-ж
chars 3 99
join a,b,c from abc
kinds True True False True True False
cases A a True False
surrogates True True False
range caught

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        String s = "abcabc";
        Console.WriteLine("index " + s[1] + " " + s.Length.ToString().Length + 4);

        Console.WriteLine("empty " + String.IsNullOrEmpty("") + " " + String.IsNullOrEmpty(null)
            + " " + String.IsNullOrWhiteSpace(" \t "));

        Console.WriteLine("indexof " + s.IndexOf('c') + " " + s.IndexOf('z') + " " + s.IndexOf("bc", 2)
            + " " + s.IndexOf("abc", 1));

        Console.WriteLine("last " + s.LastIndexOf('c') + " " + s.LastIndexOf("abc") + " " + s.LastIndexOf("zz"));

        Console.WriteLine("contains " + s.Contains("ca") + " " + s.Contains('z'));
        Console.WriteLine("starts " + s.StartsWith("abca") + " " + s.StartsWith('b')
            + " ends " + s.EndsWith("bc") + " " + s.EndsWith("ab"));

        String t = "  x y  ";
        Console.WriteLine("trim [" + t.Trim() + "] [" + t.TrimStart() + "] [" + t.TrimEnd() + "]");

        Console.WriteLine("pad [" + "7".PadLeft(4) + "] [" + "7".PadRight(4, '.') + "]".Replace("..", "..."));
        Console.WriteLine("insert " + "abc".Insert(1, "-b").Remove(3, 1) + " remove " + "abc".Remove(1, 1));

        Console.WriteLine("replace " + "a-b-c".Replace("x", "y") + "|" + "a-b-c".Replace("-", "") + "|"
            + "x".Replace('x', 'x'));

        String[] parts = "a,b,c".Split(',');
        Console.WriteLine("split " + parts.Length + " " + String.Join("|", parts));

        String[] parts2 = "ab,,c".Split(",");
        Console.WriteLine("split2 " + parts2.Length + " [" + parts2[0] + "][" + parts2[1] + "][" + parts2[2] + "]");

        Console.WriteLine("upper " + "abc-ü-ж".ToUpper() + " lower " + "ABC-Ü-Ж".ToLower());

        Char[] chars = "abc".ToCharArray();
        Console.WriteLine("chars " + chars.Length + " " + (Int32)chars[2]);
        Console.WriteLine("join " + String.Join(",", new String[] { "a", "b", "c" }) + " from "
            + String.FromChars(chars));

        Console.WriteLine("kinds " + Char.IsLetter('中') + " " + Char.IsDigit('7') + " " + Char.IsDigit('x')
            + " " + Char.IsWhiteSpace('\t') + " " + Char.IsLetterOrDigit('9') + " " + Char.IsLetter('-'));

        Console.WriteLine("cases " + Char.ToUpper('a') + " " + Char.ToLower('A') + " " + Char.IsUpper('Q')
            + " " + Char.IsUpper('q'));

        String face = "😀";
        Console.WriteLine("surrogates " + Char.IsHighSurrogate(face[0]) + " " + Char.IsLowSurrogate(face[1])
            + " " + Char.IsSurrogate('a'));

        try
        {
            Char bad = s[9];
            Console.WriteLine("no " + bad);
        }
        catch (OutOfRangeException)
        {
            Console.WriteLine("range caught");
        }
    }
}
