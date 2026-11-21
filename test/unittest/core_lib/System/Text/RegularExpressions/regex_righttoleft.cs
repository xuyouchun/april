// Regex: RegexOptions.RightToLeft (the search starts at the end of the text and goes to the left; the parts of the pattern
// are matched from the right to the left), and the look behind of any length (it is matched from the right to the left).

/* EXPECT

\d+ | ab12cd345ef | [345]@6
\d+ | ab12cd345ef | [12]@2
abc | abcabc | [abc]@3
x | abc | -
 | abc | []@3
a|ab | ab | [ab]@0
ab|a | ab | [ab]@0
\d+ | 123 | [123]@0
\d+? | 123 | [3]@2
\d{2} | 12345 | [45]@3
a.* | xaybza | [aybza]@1
a.*? | xaybza | [a]@5
a.+ | xaybza | [aybza]@1
(\w)+ | abc | [abc]@0
(ab)+ | ababab | [ababab]@0
a{2,3} | aaaa | [aaa]@1
a{2,3}? | aaaa | [aa]@2
(?:ab|a)+ | aab | [aab]@0
groups b2 b 2 2
named y 22 y 22
loop 1 3 3
^\w+ | ab cd | [ab]@0
\w+$ | ab cd | [cd]@3
\bcd | ab cd | [cd]@3
(?m)^\w | ab
cd | [c]@3
\1(a) | xaa | [aa]@1
(a)\1 | xaa | -
(?<n>a)\k<n> | aa | -
(?i)A | xa | [a]@1
(a)(?(1)b|c) | ab | -
matches 3: 333@6 22@3 1@1
empty 4: @3 x@1 @1 @0
overlap 2: aa@3 aa@1
next 3@4 2@2 1@0 
startat 2 1 1 False 4
startat2 0 3
replace a#b#c# a1b2c# a#b2c3
evaluator a<1>b<2>c<3> 321
split 3:a|b|c 2:a,b|c 2:a|b,c 3:a|,|b
static a#b# True 2 3:a|b|c
(?<=a+)b | aaab | [b]@3
(?<=a+)b | b | -
(?<=ab|c)x | abx | [x]@2
(?<=ab|c)x | cx | [x]@1
(?<=ab|c)x | bx | -
(?<!\d+)x | 12x x | [x]@4
(?<=\d+\.\d*)x | 3.14x | [x]@4
(?<=\b\w+\s)end | the end | [end]@4
(?<=^\w+)@ | name@host | [@]@4
(?<=a.*b)c | a123bc | [c]@5
(?<=a.*?b)c | a123bc | [c]@5
(?<=\1(a))b | aab | [b]@2
(?<=(a)\1)b | aab | -
(?<=(?<n>a)\k<n>)b | aab | -
(?<=\k<n>(?<n>a))b | aab | [b]@2
(?<=(a|b)+)c | abc | [c]@2
(?<![a-z]+)\d | ab1 2 | [2]@4
(?<=a(?=b))b | ab | [b]@1
(?<=(?<=a)b)c | abc | [c]@2
(?<=(?<!x)a)b | xab ab | [b]@5
behind big 4
(?<=a+)b | aab | [b]@2
\w+(?<=\d) | ab12 cd | [ab12]@0
(?<=x)\w+ | xab xcd | [cd]@5
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    static void M(String pattern, String text, RegexOptions options)
    {
        Match m = new Regex(pattern, options).Match(text);
        Console.WriteLine(pattern + " | " + text + " | " + (m.Success ? "[" + m.Value + "]@" + m.Index : "-"));
    }

    static String All(Regex r, String text)
    {
        MatchCollection ms = r.Matches(text);
        String s = ms.Count + ":";
        for (Int32 i = 0; i < ms.Count; i++)
            s += " " + ms[i].Value + "@" + ms[i].Index;

        return s;
    }

    static String Join(String[] parts)
    {
        String s = parts.Length + ":";
        for (Int32 i = 0; i < parts.Length; i++)
            s += (i > 0 ? "|" : "") + parts[i];

        return s;
    }

    [EntryPoint]
    public static void Main()
    {
        RegexOptions rtl = RegexOptions.RightToLeft;

        // The first match is the one at the right.
        M("\\d+", "ab12cd345ef", rtl);
        M("\\d+", "ab12cd345ef", RegexOptions.None);
        M("abc", "abcabc", rtl);
        M("x", "abc", rtl);
        M("", "abc", rtl);
        M("a|ab", "ab", rtl);
        M("ab|a", "ab", rtl);

        // Greedy and lazy loops take the characters from the right.
        M("\\d+", "123", rtl);
        M("\\d+?", "123", rtl);
        M("\\d{2}", "12345", rtl);
        M("a.*", "xaybza", rtl);
        M("a.*?", "xaybza", rtl);
        M("a.+", "xaybza", rtl);
        M("(\\w)+", "abc", rtl);
        M("(ab)+", "ababab", rtl);
        M("a{2,3}", "aaaa", rtl);
        M("a{2,3}?", "aaaa", rtl);
        M("(?:ab|a)+", "aab", rtl);

        // The groups are made from the right.
        Match g = new Regex("(\\w)(\\d)", rtl).Match("a1b2");
        Console.WriteLine("groups " + g.Value + " " + g.Groups[1].Value + " " + g.Groups[2].Value + " " + g.Groups[1].Index);
        Match h = new Regex("(?<w>\\w+) (?<d>\\d+)", rtl).Match("x 1 y 22");
        Console.WriteLine("named " + h.Value + " " + h.Groups["w"].Value + " " + h.Groups["d"].Value);
        Match loop = new Regex("(\\d)+", rtl).Match("a123");
        Console.WriteLine("loop " + loop.Groups[1].Value + " " + loop.Groups[1].Captures.Count + " " + loop.Groups[1].Captures[0].Value);

        // The anchors and the references.
        M("^\\w+", "ab cd", rtl);
        M("\\w+$", "ab cd", rtl);
        M("\\bcd", "ab cd", rtl);
        M("(?m)^\\w", "ab\ncd", rtl);
        M("\\1(a)", "xaa", rtl);
        M("(a)\\1", "xaa", rtl);
        M("(?<n>a)\\k<n>", "aa", rtl);
        M("(?i)A", "xa", rtl);
        M("(a)(?(1)b|c)", "ab", rtl);

        // The matches, from the right, and the next one.
        Console.WriteLine("matches " + All(new Regex("\\d+", rtl), "a1b22c333"));
        Console.WriteLine("empty " + All(new Regex("x*", rtl), "axb"));
        Console.WriteLine("overlap " + All(new Regex("aa", rtl), "aaaaa"));
        Match first = new Regex("\\d", rtl).Match("1a2b3");
        String chain = "";
        while (first.Success)
        {
            chain += first.Value + "@" + first.Index + " ";
            first = first.NextMatch();
        }

        Console.WriteLine("next " + chain);

        // The index where the search starts: the match is at its left.
        Regex digit = new Regex("\\d", rtl);
        Console.WriteLine("startat " + digit.Match("1a2b3", 4).Value + " " + digit.Match("1a2b3", 2).Value + " " + digit.Match("1a2b3", 1).Value + " " + digit.IsMatch("1a2b3", 0) + " " + digit.Match("abc12", 5).Index);
        Console.WriteLine("startat2 " + new Regex("abc", rtl).Match("abcabc", 5).Index + " " + new Regex("abc", rtl).Match("abcabc", 6).Index);

        // Replace and Split.
        Console.WriteLine("replace " + new Regex("\\d", rtl).Replace("a1b2c3", "#") + " " + new Regex("\\d", rtl).Replace("a1b2c3", "#", 1) + " " + new Regex("\\d", rtl).Replace("a1b2c3", "#", 2, 3));
        String[] order = new String[1];
        order[0] = "";
        String text = new Regex("\\d", rtl).Replace("a1b2c3", m => { order[0] += m.Value; return "<" + m.Value + ">"; });
        Console.WriteLine("evaluator " + text + " " + order[0]);
        Console.WriteLine("split " + Join(new Regex(",", rtl).Split("a,b,c")) + " " + Join(new Regex(",", rtl).Split("a,b,c", 2)) + " " + Join(new Regex(",").Split("a,b,c", 2)) + " " + Join(new Regex("(,)", rtl).Split("a,b")));
        Console.WriteLine("static " + Regex.Replace("a1b2", "\\d", "#", rtl) + " " + Regex.IsMatch("ab", "b$", rtl) + " " + Regex.Matches("a1b2", "\\d", rtl).Count + " " + Join(Regex.Split("a-b-c", "-", rtl)));

        // A look behind of any length.
        M("(?<=a+)b", "aaab", RegexOptions.None);
        M("(?<=a+)b", "b", RegexOptions.None);
        M("(?<=ab|c)x", "abx", RegexOptions.None);
        M("(?<=ab|c)x", "cx", RegexOptions.None);
        M("(?<=ab|c)x", "bx", RegexOptions.None);
        M("(?<!\\d+)x", "12x x", RegexOptions.None);
        M("(?<=\\d+\\.\\d*)x", "3.14x", RegexOptions.None);
        M("(?<=\\b\\w+\\s)end", "the end", RegexOptions.None);
        M("(?<=^\\w+)@", "name@host", RegexOptions.None);
        M("(?<=a.*b)c", "a123bc", RegexOptions.None);
        M("(?<=a.*?b)c", "a123bc", RegexOptions.None);
        M("(?<=\\1(a))b", "aab", RegexOptions.None);
        M("(?<=(a)\\1)b", "aab", RegexOptions.None);
        M("(?<=(?<n>a)\\k<n>)b", "aab", RegexOptions.None);
        M("(?<=\\k<n>(?<n>a))b", "aab", RegexOptions.None);
        M("(?<=(a|b)+)c", "abc", RegexOptions.None);
        M("(?<![a-z]+)\\d", "ab1 2", RegexOptions.None);
        M("(?<=a(?=b))b", "ab", RegexOptions.None);
        M("(?<=(?<=a)b)c", "abc", RegexOptions.None);
        M("(?<=(?<!x)a)b", "xab ab", RegexOptions.None);

        // The groups of a look behind, and a look behind in a pattern that goes to the left.
        Match behind = new Regex("(?<=(\\w+) )end").Match("the big end");
        Console.WriteLine("behind " + behind.Groups[1].Value + " " + behind.Groups[1].Index);
        M("(?<=a+)b", "aab", rtl);
        M("\\w+(?<=\\d)", "ab12 cd", RegexOptions.None);
        M("(?<=x)\\w+", "xab xcd", rtl);

        Console.WriteLine("done");
    }
}
