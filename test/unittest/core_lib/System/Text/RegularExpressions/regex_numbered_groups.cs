// Regex: the groups that have a number for a name, (?<5>a): they keep the number, the groups that have no name take the numbers
// that are left, the numbers can have holes (the groups are 0 and 5), and the references, the replacements and the balancing
// groups work with them.

/* EXPECT

holes 3: 0 3 5 | 3: 0 3 5 | 3
by number ab True False False [] 5 0
lookups 5 [] 5 -1 b
order 0=ab 3=b 5=a 
mixed 4: 0 1 2 3 | abc
named 4: 0 1 2 3 | 4: 0 1 2 x | cba a
later 5: 0 1 2 3 4 | 5: 0 1 2 3 y | cbda
twice 2 b 2
(?<2>a)\2 | xaa | [aa]@1
(?<2>a)\k<2> | aa | [aa]@0
(?<3>a)(?<2>b)\3\2 | abab | [abab]@0
(?<10>a)\10 | aa | [aa]@0
(a)\10 | a | [a]@0
(?<2>a)?(?(2)b|c) | ab | [ab]@0
(?<2>a)?(?(2)b|c) | c | [c]@0
(?<n>a)(?<1>b)\1 | abb | [abb]@0
(?<2>a)(?<n>b)\k<n>\k<2> | abba | [abba]@0
replace [a]b [$1]b [aa]b a [a]b
split 3: a - b
balance False True []@1
cancel True False
(?<-2>x)(?<2>y) | xy | -
^(?:(?<1>\()|(?<-1>\))|[^()])*(?(1)(?!))$ | (()()) | [(()())]@0
^(?:(?<1>\()|(?<-1>\))|[^()])*(?(1)(?!))$ | (() | -
count 3 0 1
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    static String Join(Int32[] numbers)
    {
        String s = numbers.Length + ":";
        for (Int32 i = 0; i < numbers.Length; i++)
            s += " " + numbers[i];

        return s;
    }

    static String Join(String[] names)
    {
        String s = names.Length + ":";
        for (Int32 i = 0; i < names.Length; i++)
            s += " " + names[i];

        return s;
    }

    static void M(String pattern, String text)
    {
        Match m = new Regex(pattern).Match(text);
        Console.WriteLine(pattern + " | " + text + " | " + (m.Success ? "[" + m.Value + "]@" + m.Index : "-"));
    }

    [EntryPoint]
    public static void Main()
    {
        // The numbers have holes.
        Regex holes = new Regex("(?<5>a)(?<3>b)");
        Match m = holes.Match("ab");
        Console.WriteLine("holes " + Join(holes.GetGroupNumbers()) + " | " + Join(holes.GetGroupNames()) + " | " + m.Groups.Count);
        Console.WriteLine("by number " + m.Groups[5].Value + m.Groups[3].Value + " " + m.Groups[5].Success + " " + m.Groups[1].Success + " " + m.Groups[4].Success + " [" + m.Groups[1].Value + "] " + m.Groups[5].Name + " " + m.Groups[0].Name);
        Console.WriteLine("lookups " + holes.GroupNameFromNumber(5) + " [" + holes.GroupNameFromNumber(4) + "] " + holes.GroupNumberFromName("5") + " " + holes.GroupNumberFromName("4") + " " + m.Groups["3"].Value);

        String order = "";
        foreach (Group g in m.Groups)
            order += g.Name + "=" + g.Value + " ";

        Console.WriteLine("order " + order);

        // The groups that have no name take the numbers that are left, the named ones come after.
        Regex mixed = new Regex("(a)(?<2>b)(c)");
        Console.WriteLine("mixed " + Join(mixed.GetGroupNumbers()) + " | " + mixed.Match("abc").Groups[1].Value + mixed.Match("abc").Groups[2].Value + mixed.Match("abc").Groups[3].Value);

        Regex named = new Regex("(?<x>a)(?<2>b)(c)");
        Match nm = named.Match("abc");
        Console.WriteLine("named " + Join(named.GetGroupNumbers()) + " | " + Join(named.GetGroupNames()) + " | " + nm.Groups[1].Value + nm.Groups[2].Value + nm.Groups[3].Value + " " + nm.Groups["x"].Value);

        Regex later = new Regex("(?<y>a)(b)(?<1>c)(d)");
        Match lm = later.Match("abcd");
        Console.WriteLine("later " + Join(later.GetGroupNumbers()) + " | " + Join(later.GetGroupNames()) + " | " + lm.Groups[1].Value + lm.Groups[2].Value + lm.Groups[3].Value + lm.Groups[4].Value);

        // A number that is used twice is one group.
        Match twice = new Regex("(?<2>a)|(?<2>b)").Match("b");
        Console.WriteLine("twice " + twice.Groups.Count + " " + twice.Groups[2].Value + " " + new Regex("(?<2>a)|(?<2>b)").Matches("ab").Count);

        // The references and the conditions.
        M("(?<2>a)\\2", "xaa");
        M("(?<2>a)\\k<2>", "aa");
        M("(?<3>a)(?<2>b)\\3\\2", "abab");
        M("(?<10>a)\\10", "aa");
        M("(a)\\10", "a" + (Char)8);
        M("(?<2>a)?(?(2)b|c)", "ab");
        M("(?<2>a)?(?(2)b|c)", "c");
        M("(?<n>a)(?<1>b)\\1", "abb");
        M("(?<2>a)(?<n>b)\\k<n>\\k<2>", "abba");

        // The replacement and the split.
        Console.WriteLine("replace " + Regex.Replace("ab", "(?<5>a)", "[$5]") + " " + Regex.Replace("ab", "(?<5>a)", "[$1]") + " " + Regex.Replace("ab", "(?<5>a)", "[${5}$&]") + " " + Regex.Replace("ab", "(?<5>a)(?<3>b)", "$+") + " " + Regex.Replace("ab", "(?<5>a)", "[$05]"));
        String[] parts = Regex.Split("a-b", "(?<7>-)");
        Console.WriteLine("split " + Join(parts));

        // The balancing groups with numbers.
        Match balanced = new Regex("(?<2>a)(?<3-2>b)").Match("ab");
        Console.WriteLine("balance " + balanced.Groups[2].Success + " " + balanced.Groups[3].Success + " [" + balanced.Groups[3].Value + "]@" + balanced.Groups[3].Index);
        Match cancel = new Regex("(?<2>a)(?<-2>b)(?(2)x|y)").Match("aby");
        Console.WriteLine("cancel " + cancel.Success + " " + cancel.Groups[2].Success);
        M("(?<-2>x)(?<2>y)", "xy");
        M("^(?:(?<1>\\()|(?<-1>\\))|[^()])*(?(1)(?!))$", "(()())");
        M("^(?:(?<1>\\()|(?<-1>\\))|[^()])*(?(1)(?!))$", "(()");

        // The group names of a match.
        Match last = new Regex("(?<9>a)(?<4>b)").Match("ab");
        Console.WriteLine("count " + last.Groups.Count + " " + last.Groups[9].Index + " " + last.Groups[4].Index);

        Console.WriteLine("done");
    }
}
