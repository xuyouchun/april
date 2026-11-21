// Regex: the references to a group (\1, \k<name>), the assertions (look ahead, look behind of a fixed length),
// the atomic groups, the conditions, and how they work with the groups and the backtracking.

/* EXPECT

(a)\1 | xaa | [aa]@1
(a)\1 | xab | -
(\w)\1 | abccd | [cc]@2
(\w+) \1 | say hello hello now | [hello hello]@4
(?<q>['"]).*?\k<q> | say 'hi' now | ['hi']@4
(?<q>['"]).*?\k'q' | say "hi' there" now | ["hi' there"]@4
(?<n>a)\k<1> | aa | [aa]@0
(a)(b)\2\1 | abba | [abba]@0
(a)|b\1 | b | -
(a)?b\1 | bab | -
(a*)b\1 | aabaa | [aabaa]@0
^(a+)\1$ | aaaa | [aaaa]@0
^(a+)\1$ | aaa | -
(a)\1{2} | aaa | [aaa]@0
(ab)*\1 | ababab | [ababab]@0
(a)\10 | a<8> | [a<8>]@0
(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)\10 | abcdefghijj | [abcdefghijj]@0
\101 | A | [A]@0
(a)\1 | aA | [aA]@0
(?i)(a)\1 | Aa | [Aa]@0
(a)\1 | aA | -
a(?=b) | ac ab | [a]@3
a(?!b) | ab ac | [a]@3
a(?=b)b | ab | [ab]@0
(?=a)\w | ba | [a]@1
(?!a)\w | ab | [b]@1
\w+(?=ing) | running | [runn]@0
\b\w+(?<!ing)\b | sing walking talk | [talk]@13
^(?=.*\d)(?=.*[a-z]).{4,}$ | abc1 | [abc1]@0
^(?=.*\d)(?=.*[a-z]).{4,}$ | abcd | -
(?=(\w+))\1: | abc: | [abc:]@0
(?!(a))b | b | [b]@0
x(?=) | x | [x]@0
(?=a)*b | ab | [b]@1
(?<=\$)\d+ | cost $42 | [42]@6
(?<!\$)\b\d+ | $42 17 | [17]@4
(?<=ab|cd)x | cdx | [x]@2
(?<=ab|cd)x | bdx | -
(?<=a{2}[bc]\d)x | aac5x | [x]@4
(?<=^)a | a | [a]@0
(?<=\bfoo)bar | foobar | [bar]@3
(?<![a-z])\d | a1 2 | [2]@3
(?<=(a)(b))c | abc | [c]@2
(?<=a) | ba | []@2
(?<=a)(?<=a)b | ab | [b]@1
(?<!^)a | aa | [a]@1
(?<=\d{3})x | 12x 123x | [x]@7
kept 1 123 1
behind ab 0
negative False c
undone ac False
(?>a+)b | aaab | [aaab]@0
(?>a+)ab | aaab | -
(?>a|ab)c | abc | -
(?:a|ab)c | abc | [abc]@0
(?>.*)x | abx | -
(?>(a+))b | aab | [aab]@0
(?>a*)*b | aab | [aab]@0
a(?>bc|b)c | abcc | [abcc]@0
a(?>bc|b)c | abc | -
(?>(\d+))\1 | 1212 | -
atomic abac 2
(a)?(?(1)b|c) | ab c | [ab]@0
(a)?(?(1)b|c) | c | [c]@0
(a)?(?(1)b)c | bc | [c]@1
^(\()?\d+(?(1)\))$ | (12) | [(12)]@0
^(\()?\d+(?(1)\))$ | 12 | [12]@0
^(\()?\d+(?(1)\))$ | (12 | -
^(\()?\d+(?(1)\))$ | 12) | -
(?<open><)?\w+(?(open)>) | <tag> | [<tag>]@0
^(?(?=\d)\d+|[a-z]+)$ | 123 | [123]@0
^(?(?=\d)\d+|[a-z]+)$ | abc | [abc]@0
^(?(?=\d)\d+|[a-z]+)$ | 1ab | -
(?(?!a)b|ac) | ac | [ac]@0
^(?(?<=x)y|z) | z | [z]@0
(a)(?(1)(b)|(c)) | ab | [ab]@0
cond False c
^(?<w>\w+)(?: (?<w2>\k<w>))+$ | ab ab ab | [ab ab ab]@0
(?<=(?<a>x))y\k<a> | xyx | [yx]@1
\b(\w)(\w*)\1\b | abca bab | [abca]@0
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    static String Show(String s)
    {
        return s.Replace("\n", "\\n").Replace(((Char)8).ToString(), "<8>");
    }

    static void M(String pattern, String text)
    {
        MO(pattern, text, RegexOptions.None);
    }

    static void MO(String pattern, String text, RegexOptions options)
    {
        Match m = new Regex(pattern, options).Match(text);
        String result = m.Success ? "[" + Show(m.Value) + "]@" + m.Index : "-";
        Console.WriteLine(Show(pattern) + " | " + Show(text) + " | " + result);
    }

    [EntryPoint]
    public static void Main()
    {
        // The references to a group.
        M("(a)\\1", "xaa");
        M("(a)\\1", "xab");
        M("(\\w)\\1", "abccd");
        M("(\\w+) \\1", "say hello hello now");
        M("(?<q>['\"]).*?\\k<q>", "say 'hi' now");
        M("(?<q>['\"]).*?\\k'q'", "say \"hi' there\" now");
        M("(?<n>a)\\k<1>", "aa");
        M("(a)(b)\\2\\1", "abba");
        M("(a)|b\\1", "b");
        M("(a)?b\\1", "bab");
        M("(a*)b\\1", "aabaa");
        M("^(a+)\\1$", "aaaa");
        M("^(a+)\\1$", "aaa");
        M("(a)\\1{2}", "aaa");
        M("(ab)*\\1", "ababab");
        M("(a)\\10", "a" + (Char)8);
        M("(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)\\10", "abcdefghijj");
        M("\\101", "A");
        MO("(a)\\1", "aA", RegexOptions.IgnoreCase);
        MO("(?i)(a)\\1", "Aa", RegexOptions.None);
        MO("(a)\\1", "aA", RegexOptions.None);

        // A look ahead: it matches no character, and the groups that it has are kept.
        M("a(?=b)", "ac ab");
        M("a(?!b)", "ab ac");
        M("a(?=b)b", "ab");
        M("(?=a)\\w", "ba");
        M("(?!a)\\w", "ab");
        M("\\w+(?=ing)", "running");
        M("\\b\\w+(?<!ing)\\b", "sing walking talk");
        M("^(?=.*\\d)(?=.*[a-z]).{4,}$", "abc1");
        M("^(?=.*\\d)(?=.*[a-z]).{4,}$", "abcd");
        M("(?=(\\w+))\\1:", "abc:");
        M("(?!(a))b", "b");
        M("x(?=)", "x");
        M("(?=a)*b", "ab");

        // A look behind: a part that has a fixed length.
        M("(?<=\\$)\\d+", "cost $42");
        M("(?<!\\$)\\b\\d+", "$42 17");
        M("(?<=ab|cd)x", "cdx");
        M("(?<=ab|cd)x", "bdx");
        M("(?<=a{2}[bc]\\d)x", "aac5x");
        M("(?<=^)a", "a");
        M("(?<=\\bfoo)bar", "foobar");
        M("(?<![a-z])\\d", "a1 2");
        M("(?<=(a)(b))c", "abc");
        M("(?<=a)", "ba");
        M("(?<=a)(?<=a)b", "ab");
        M("(?<!^)a", "aa");
        M("(?<=\\d{3})x", "12x 123x");

        // The groups of an assertion.
        Match look = new Regex("(?=(\\d+))\\d").Match("a123");
        Console.WriteLine("kept " + look.Value + " " + look.Groups[1].Value + " " + look.Groups[1].Index);
        Match behind = new Regex("(?<=(\\w)(\\w))x").Match("abx");
        Console.WriteLine("behind " + behind.Groups[1].Value + behind.Groups[2].Value + " " + behind.Groups[1].Index);
        Match negative = new Regex("(?!(a))b|(c)").Match("c");
        Console.WriteLine("negative " + negative.Groups[1].Success + " " + negative.Groups[2].Value);
        Match undone = new Regex("(?:(?=(a))ab|ac)").Match("ac");
        Console.WriteLine("undone " + undone.Value + " " + undone.Groups[1].Success);

        // An atomic group: no choice that it made is tried again.
        M("(?>a+)b", "aaab");
        M("(?>a+)ab", "aaab");
        M("(?>a|ab)c", "abc");
        M("(?:a|ab)c", "abc");
        M("(?>.*)x", "abx");
        M("(?>(a+))b", "aab");
        M("(?>a*)*b", "aab");
        M("a(?>bc|b)c", "abcc");
        M("a(?>bc|b)c", "abc");
        M("(?>(\\d+))\\1", "1212");
        Match atomic = new Regex("(?>(a)|b)+c").Match("abac");
        Console.WriteLine("atomic " + atomic.Value + " " + atomic.Groups[1].Captures.Count);

        // A condition on a group, and on an assertion.
        M("(a)?(?(1)b|c)", "ab c");
        M("(a)?(?(1)b|c)", "c");
        M("(a)?(?(1)b)c", "bc");
        M("^(\\()?\\d+(?(1)\\))$", "(12)");
        M("^(\\()?\\d+(?(1)\\))$", "12");
        M("^(\\()?\\d+(?(1)\\))$", "(12");
        M("^(\\()?\\d+(?(1)\\))$", "12)");
        M("(?<open><)?\\w+(?(open)>)", "<tag>");
        M("^(?(?=\\d)\\d+|[a-z]+)$", "123");
        M("^(?(?=\\d)\\d+|[a-z]+)$", "abc");
        M("^(?(?=\\d)\\d+|[a-z]+)$", "1ab");
        M("(?(?!a)b|ac)", "ac");
        M("^(?(?<=x)y|z)", "z");
        M("(a)(?(1)(b)|(c))", "ab");
        Match cond = new Regex("(a)?(?(1)(b)|(c))").Match("c");
        Console.WriteLine("cond " + cond.Groups[2].Success + " " + cond.Groups[3].Value);

        // Many together.
        M("^(?<w>\\w+)(?: (?<w2>\\k<w>))+$", "ab ab ab");
        M("(?<=(?<a>x))y\\k<a>", "xyx");
        M("\\b(\\w)(\\w*)\\1\\b", "abca bab");

        Console.WriteLine("done");
    }
}
