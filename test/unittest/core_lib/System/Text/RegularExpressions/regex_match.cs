// Regex: what a pattern matches. Each line is the pattern, the text and the first match: [value]@index, or - when
// there is none. \n and \t in a value are written as the two characters.

/* EXPECT

abc | xxabcxx | [abc]@2
abc | ab | -
a.c | abc | [abc]@0
a.c | a\nc | -
a\.b | axb a.b | [a.b]@4
\(x\) | f(x) | [(x)]@1
\x41\u0042 | zAB | [AB]@1
a\tb | a\tb | [a\tb]@0
\0 | a\0b | [\0]@1
 | abc | []@0
^abc | abc | [abc]@0
^b | ab | -
c$ | abc | [c]@2
c$ | abc\n | [c]@2
c\z | abc\n | -
c\Z | abc\n | [c]@2
\Ab | ab | -
^b | a\nb | [b]@2
a$ | a\nb | [a]@0
^b | a\nb | -
\bfoo\b | a foo b | [foo]@2
\bfoo\b | afoo | -
\Boo | foo | [oo]@1
[abc]+ | xxbcax | [bca]@2
[^abc]+ | abxyc | [xy]@2
[a-c-[b]]+ | abcab | [a]@0
[a-z-[aeiou]]+ | aeixyzou | [xyz]@3
[\d.]+ | v1.25x | [1.25]@1
[]a]+ | x]a] | []a]]@1
[a\-z]+ | x-az | [-az]@1
[\w-]+ | !a-b! | [a-b]@1
[^\s]+ |   ab cd | [ab]@2
[\x41-\x43]+ | xABCD | [ABC]@1
\d+ | ab123c | [123]@2
\D+ | 12ab34 | [ab]@2
\w+ |   foo_1  | [foo_1]@2
\W+ | ab, cd | [, ]@2
\s+ | a \t b | [ \t ]@1
\S+ |   ab cd | [ab]@2
a.*b | aXbXb | [aXbXb]@0
a.*?b | aXbXb | [aXb]@0
a+ | baaac | [aaa]@1
a+? | baaac | [a]@1
ab?c | ac abc | [ac]@0
ab??c | abc | [abc]@0
a{2} | aaaa | [aa]@0
a{2,} | aaaa | [aaaa]@0
a{2,3} | aaaa | [aaa]@0
a{2,3}? | aaaa | [aa]@0
a{0}b | ab | [b]@1
a{,3} | a{,3} | [a{,3}]@0
a{x} | a{x} | [a{x}]@0
a{2 | a{2 | [a{2]@0
(ab)+ | xababab | [ababab]@1
(ab){2} | ababab | [abab]@0
(a|b)*c | abbac | [abbac]@0
x* | aaa | []@0
(a*)+ | b | []@0
(a*)*b | aab | [aab]@0
(a?)*b | aab | [aab]@0
(a|)+b | aab | [aab]@0
(?:a*)*b | aab | [aab]@0
a|ab | ab | [a]@0
ab|a | ab | [ab]@0
(a|ab)(c|bcd) | abcd | [abcd]@0
cat|dog|bird | hotdog | [dog]@3
^(a|b)$ | b | [b]@0
x(|y)z | xz | [xz]@0
hello | say HELLO | [HELLO]@4
[a-c]+ | xABCx | [ABC]@1
[^a-c]+ | ABCxyz | [xyz]@3
(?i)abc | xABC | [ABC]@1
a(?i)b | aB | [aB]@0
a(?i)b | AB | -
a(?i:b)c | aBc | [aBc]@0
a(?i:b)c | aBC | -
(?i)a(?-i)b | Ab | [Ab]@0
(?i)a(?-i)b | AB | -
.+ | ab\ncd | [ab]@0
.+ | ab\ncd | [ab\ncd]@0
(?s).+ | ab\ncd | [ab\ncd]@0
(?m)^b$ | a\nb\nc | [b]@2
a b # comment\n c | abc | [abc]@0
a [ ] b | a b | [a b]@0
a\ b | a b | [a b]@0
a(?#comment)b | ab | [ab]@0
a b | ab | -
a b | a b | [a b]@0
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    static String Show(String s)
    {
        return s.Replace("\n", "\\n").Replace("\t", "\\t").Replace("\0", "\\0");
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
        // Characters.
        M("abc", "xxabcxx");
        M("abc", "ab");
        M("a.c", "abc");
        M("a.c", "a\nc");
        M("a\\.b", "axb a.b");
        M("\\(x\\)", "f(x)");
        M("\\x41\\u0042", "zAB");
        M("a\\tb", "a\tb");
        M("\\0", "a\0b");
        M("", "abc");

        // Anchors.
        M("^abc", "abc");
        M("^b", "ab");
        M("c$", "abc");
        M("c$", "abc\n");
        M("c\\z", "abc\n");
        M("c\\Z", "abc\n");
        M("\\Ab", "ab");
        MO("^b", "a\nb", RegexOptions.Multiline);
        MO("a$", "a\nb", RegexOptions.Multiline);
        M("^b", "a\nb");
        M("\\bfoo\\b", "a foo b");
        M("\\bfoo\\b", "afoo");
        M("\\Boo", "foo");

        // Classes.
        M("[abc]+", "xxbcax");
        M("[^abc]+", "abxyc");
        M("[a-c-[b]]+", "abcab");
        M("[a-z-[aeiou]]+", "aeixyzou");
        M("[\\d.]+", "v1.25x");
        M("[]a]+", "x]a]");
        M("[a\\-z]+", "x-az");
        M("[\\w-]+", "!a-b!");
        M("[^\\s]+", "  ab cd");
        M("[\\x41-\\x43]+", "xABCD");
        M("\\d+", "ab123c");
        M("\\D+", "12ab34");
        M("\\w+", "  foo_1 ");
        M("\\W+", "ab, cd");
        M("\\s+", "a \t b");
        M("\\S+", "  ab cd");

        // Quantifiers.
        M("a.*b", "aXbXb");
        M("a.*?b", "aXbXb");
        M("a+", "baaac");
        M("a+?", "baaac");
        M("ab?c", "ac abc");
        M("ab??c", "abc");
        M("a{2}", "aaaa");
        M("a{2,}", "aaaa");
        M("a{2,3}", "aaaa");
        M("a{2,3}?", "aaaa");
        M("a{0}b", "ab");
        M("a{,3}", "a{,3}");
        M("a{x}", "a{x}");
        M("a{2", "a{2");
        M("(ab)+", "xababab");
        M("(ab){2}", "ababab");
        M("(a|b)*c", "abbac");
        M("x*", "aaa");
        M("(a*)+", "b");
        M("(a*)*b", "aab");
        M("(a?)*b", "aab");
        M("(a|)+b", "aab");
        M("(?:a*)*b", "aab");

        // Alternation: the first one that leads to a match, not the longest.
        M("a|ab", "ab");
        M("ab|a", "ab");
        M("(a|ab)(c|bcd)", "abcd");
        M("cat|dog|bird", "hotdog");
        M("^(a|b)$", "b");
        M("x(|y)z", "xz");

        // Options.
        MO("hello", "say HELLO", RegexOptions.IgnoreCase);
        MO("[a-c]+", "xABCx", RegexOptions.IgnoreCase);
        MO("[^a-c]+", "ABCxyz", RegexOptions.IgnoreCase);
        M("(?i)abc", "xABC");
        M("a(?i)b", "aB");
        M("a(?i)b", "AB");
        M("a(?i:b)c", "aBc");
        M("a(?i:b)c", "aBC");
        M("(?i)a(?-i)b", "Ab");
        M("(?i)a(?-i)b", "AB");
        MO(".+", "ab\ncd", RegexOptions.None);
        MO(".+", "ab\ncd", RegexOptions.Singleline);
        M("(?s).+", "ab\ncd");
        M("(?m)^b$", "a\nb\nc");
        MO("a b # comment\n c", "abc", RegexOptions.IgnorePatternWhitespace);
        MO("a [ ] b", "a b", RegexOptions.IgnorePatternWhitespace);
        MO("a\\ b", "a b", RegexOptions.IgnorePatternWhitespace);
        M("a(?#comment)b", "ab");
        MO("a b", "ab", RegexOptions.None);
        MO("a b", "a b", RegexOptions.None);

        Console.WriteLine("done");
    }
}
