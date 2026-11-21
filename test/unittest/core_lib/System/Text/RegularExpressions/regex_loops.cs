// Regex: the loops of one character (a* \d+ .{2,5} [a-z]+?) that give the characters back one by one, the text that a match
// starts with, \G, and the long texts: the loops of one character do not use the stack for each character.

/* EXPECT

a*ab | aaab | [aaab]@0
a+ab | aaab | [aaab]@0
a*a | aaa | [aaa]@0
a*a*a | aa | [aa]@0
.*foo | xx foo yy foo zz | [xx foo yy foo]@0
.*?foo | xx foo yy foo zz | [xx foo]@0
\d{2,4}\d | 12345 | [12345]@0
\d{2,4}\d | 123 | [123]@0
\d{2,4}\d | 12 | -
\d{2,4}?\d{2} | 12345 | [1234]@0
[a-c]+c | abcabc | [abcabc]@0
[a-c]+?c | abcabc | [abc]@0
[^x]*x | abcxdef | [abcx]@0
a?a?a?aaa | aaa | [aaa]@0
a{0,2}a{0,2}b | aaab | [aaab]@0
(a*)(a+) | aaa | [aaa]@0
(a*?)(a+) | aaa | [aaa]@0
a.{1,3}?d | abcdefd | [abcd]@0
x* | aaa | []@0
x*? | aaa | []@0
x+? | aaa | -
a{0}b | ab | [b]@1
a{1,1}b | aab | [ab]@1
[a-c]+C | abcabc | [abcabc]@0
A*a | aaa | [aaa]@0
.+x | a
bx | [bx]@2
.+x | a
bx | [a
bx]@0
a*(?=b) | aab | [aa]@0
(?<=a*)b | aab | [b]@2
(?>a*)a | aaa | -
(?:a*)*b | aab | [aab]@0
^a*$ | aaa | [aaa]@0
^a*$ | aab | -
groups [aaa] []
lazy [] [aaa]
counted [123] [45]
foo | a food foo | [foo]@2
foo\d | foo foo1 | [foo1]@4
\bfoo | afoo foo | [foo]@5
^foo | foo | [foo]@0
(?m)^foo | x
foo | [foo]@2
(fo)o | xfoo | [foo]@1
foobar|foobaz | x foobaz | [foobaz]@2
foo(?=bar) | foobaz foobar | [foo]@7
FOO | a foo | [foo]@2
fo+ | a foooo | [foooo]@2
ab*c | xabbc | [abbc]@1
a\bb | ab | -
aab | aaab | [aab]@1
aaab | aaaab | [aaab]@1
G 2 1 False True False
G2 3 3 3 [1][2]x,3
G3 False True
long 400001 400001 True False 400001
long2 400001 400001 400001
nomatch False False
done

*/

import System;

using System;
using System.Text;
using System.Text.RegularExpressions;

class Program
{
    static String Repeat(String s, Int32 count)
    {
        StringBuilder sb = new StringBuilder();
        for (Int32 i = 0; i < count; i++)
            sb.Append(s);

        return sb.ToString();
    }

    static void M(String pattern, String text)
    {
        Match m = new Regex(pattern).Match(text);
        Console.WriteLine(pattern + " | " + text + " | " + (m.Success ? "[" + m.Value + "]@" + m.Index : "-"));
    }

    static void MO(String pattern, String text, RegexOptions options)
    {
        Match m = new Regex(pattern, options).Match(text);
        Console.WriteLine(pattern + " | " + text + " | " + (m.Success ? "[" + m.Value + "]@" + m.Index : "-"));
    }

    [EntryPoint]
    public static void Main()
    {
        // A loop of one character gives the characters back, one by one, until the rest matches.
        M("a*ab", "aaab");
        M("a+ab", "aaab");
        M("a*a", "aaa");
        M("a*a*a", "aa");
        M(".*foo", "xx foo yy foo zz");
        M(".*?foo", "xx foo yy foo zz");
        M("\\d{2,4}\\d", "12345");
        M("\\d{2,4}\\d", "123");
        M("\\d{2,4}\\d", "12");
        M("\\d{2,4}?\\d{2}", "12345");
        M("[a-c]+c", "abcabc");
        M("[a-c]+?c", "abcabc");
        M("[^x]*x", "abcxdef");
        M("a?a?a?aaa", "aaa");
        M("a{0,2}a{0,2}b", "aaab");
        M("(a*)(a+)", "aaa");
        M("(a*?)(a+)", "aaa");
        M("a.{1,3}?d", "abcdefd");
        M("x*", "aaa");
        M("x*?", "aaa");
        M("x+?", "aaa");
        M("a{0}b", "ab");
        M("a{1,1}b", "aab");
        MO("[a-c]+C", "abcabc", RegexOptions.IgnoreCase);
        MO("A*a", "aaa", RegexOptions.IgnoreCase);
        MO(".+x", "a\nbx", RegexOptions.None);
        MO(".+x", "a\nbx", RegexOptions.Singleline);
        M("a*(?=b)", "aab");
        M("(?<=a*)b", "aab");
        M("(?>a*)a", "aaa");
        M("(?:a*)*b", "aab");
        M("^a*$", "aaa");
        M("^a*$", "aab");

        // The characters that are given back, and the groups: the captures are those of the run that matched.
        Match m = new Regex("(a*)(a*)b").Match("aaab");
        Console.WriteLine("groups [" + m.Groups[1].Value + "] [" + m.Groups[2].Value + "]");
        Match lazy = new Regex("(a*?)(a*)b").Match("aaab");
        Console.WriteLine("lazy [" + lazy.Groups[1].Value + "] [" + lazy.Groups[2].Value + "]");
        Match counted = new Regex("(\\d{1,3})(\\d{1,3})").Match("12345");
        Console.WriteLine("counted [" + counted.Groups[1].Value + "] [" + counted.Groups[2].Value + "]");

        // The text that a match starts with: the search jumps to it.
        M("foo", "a food foo");
        M("foo\\d", "foo foo1");
        M("\\bfoo", "afoo foo");
        M("^foo", "foo");
        M("(?m)^foo", "x\nfoo");
        M("(fo)o", "xfoo");
        M("foobar|foobaz", "x foobaz");
        M("foo(?=bar)", "foobaz foobar");
        MO("FOO", "a foo", RegexOptions.IgnoreCase);
        MO("fo+", "a foooo", RegexOptions.None);
        M("ab*c", "xabbc");
        M("a\\bb", "ab");
        M("aab", "aaab");
        M("aaab", "aaaab");

        // \G is the index where the search starts, and the end of the match before when the matches go on.
        Regex g = new Regex("\\Ga");
        Console.WriteLine("G " + g.Matches("aaba").Count + " " + g.Match("baa", 1).Index + " " + g.Match("xa").Success + " " + g.Match("xa", 1).Success + " " + g.IsMatch("ba", 0));
        Console.WriteLine("G2 " + new Regex("\\G\\d").Matches("123a45").Count + " " + new Regex("\\G\\d+?").Matches("123").Count + " " + new Regex("\\G").Matches("ab").Count + " " + Regex.Replace("1,2,x,3", "\\G(\\d),?", "[$1]"));
        Console.WriteLine("G3 " + new Regex("a\\G", RegexOptions.None).IsMatch("aa") + " " + new Regex("(?<=a)\\G").Match("aa", 1).Success);

        // The long texts: a loop of one character does not push a choice for each character.
        {
            String text = Repeat("a", 400000) + "b";
            Console.WriteLine("long " + new Regex("a*b").Match(text).Length + " " + new Regex("a*ab").Match(text).Length + " " + new Regex("^.*b$").IsMatch(text) + " " + new Regex("^[^b]*$").IsMatch(text) + " " + new Regex("a+?b").Match(text).Length);
            Console.WriteLine("long2 " + new Regex("(?:a)*b").Match(text).Length + " " + new Regex("^a*.b").Match(text).Length + " " + new Regex("a*c|a*b").Match(text).Length);
        }

        // A loop that gives back every character is the cost of the text: a pattern that has no match.
        {
            String text = Repeat("a", 3000);
            Console.WriteLine("nomatch " + new Regex("^a*b").IsMatch(text) + " " + new Regex("^a*a*a*b").IsMatch(Repeat("a", 60)));
        }

        Console.WriteLine("done");
    }
}
