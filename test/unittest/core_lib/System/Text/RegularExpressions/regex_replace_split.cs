// Regex: Replace (the substitutions $1 ${name} $& $` $' $+ $_ $$), Split (the groups are in the result), Escape
// and Unescape, the static methods.

/* EXPECT

plain a#b#c
whole a<1>b<22>c a<1>b<22>c
swap 1a2b
names world hello
number a1aa0b
dollar a$|$c a$9|$x|${no}|${c
around a[a|c]c aabcc
last b [ab]
none abc 
empty -a-b-c- -a-b-c- abc!
remove abc
options x x
count bbaa aaaa bbbb bbbb
startat aabb aaab
split 3:a|b|c
groups 5:a|1|b|2|c 3:a|-|b
chars 5:|a|b|c|
edges 2:|a 2:a| 3:a||b 1: 1:abc
limit 2:a|b,c,d 3:a|b|c,d 1:a,b,c,d 4:a|b|c|d 4:a|b|c|d
splitat 3:a,b|c|d
options 3:a|b|c
evaluator a<1>b<2>c a11b2222c
evaluator2 ONE1 TWO2 THREE3 3
evaluator3 - - three one - - one two
evaluator4 HELLO WORLD a0b |a|b|
escape a\.b\*c\ \(d\)\[e]\{f}\|g\^h\$i\+j\?k\\l\#m\ n\to\np
formfeed a\ff 2 True
escape2 1\+1=2 plain []
unescape True 4 plain
roundtrip True False True
cache 40 False True False
match 12 a 2
string a+b 0 3
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
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
        // Replace.
        Console.WriteLine("plain " + Regex.Replace("a1b22c", "\\d+", "#"));
        Console.WriteLine("whole " + Regex.Replace("a1b22c", "\\d+", "<$&>") + " " + Regex.Replace("a1b22c", "\\d+", "<$0>"));
        Console.WriteLine("swap " + Regex.Replace("a1b2", "(\\w)(\\d)", "$2$1"));
        Console.WriteLine("names " + Regex.Replace("hello world", "(?<w>\\w+) (?<v>\\w+)", "${v} ${w}"));
        Console.WriteLine("number " + Regex.Replace("ab", "(a)", "${1}1$1$10"));
        Console.WriteLine("dollar " + Regex.Replace("abc", "b", "$$|$") + " " + Regex.Replace("abc", "b", "$9|$x|${no}|${"));
        Console.WriteLine("around " + Regex.Replace("abc", "b", "[$`|$']") + " " + Regex.Replace("abc", "b", "$_"));
        Console.WriteLine("last " + Regex.Replace("ab", "(a)(b)", "$+") + " " + Regex.Replace("ab", "ab", "[$+]"));
        Console.WriteLine("none " + Regex.Replace("abc", "x", "y") + " " + Regex.Replace("", "x", "y"));
        Console.WriteLine("empty " + Regex.Replace("abc", "x*", "-") + " " + Regex.Replace("abc", "", "-") + " " + Regex.Replace("abc", "$", "!"));
        Console.WriteLine("remove " + Regex.Replace("a b  c", "\\s+", ""));
        Console.WriteLine("options " + Regex.Replace("Hello HELLO", "hello", "x", RegexOptions.IgnoreCase));

        Regex r = new Regex("a");
        Console.WriteLine("count " + r.Replace("aaaa", "b", 2) + " " + r.Replace("aaaa", "b", 0) + " " + r.Replace("aaaa", "b", -1) + " " + r.Replace("aaaa", "b", 10));
        Console.WriteLine("startat " + r.Replace("aaaa", "b", -1, 2) + " " + r.Replace("aaaa", "b", 1, 3));

        // Split.
        Console.WriteLine("split " + Join(Regex.Split("a, b,c", ",\\s*")));
        Console.WriteLine("groups " + Join(Regex.Split("a1b2c", "(\\d)")) + " " + Join(Regex.Split("a-b", "(-)|(\\+)")));
        Console.WriteLine("chars " + Join(Regex.Split("abc", "")));
        Console.WriteLine("edges " + Join(Regex.Split(",a", ",")) + " " + Join(Regex.Split("a,", ",")) + " " + Join(Regex.Split("a,,b", ",")) + " " + Join(Regex.Split("", ",")) + " " + Join(Regex.Split("abc", ",")));
        Regex comma = new Regex(",");
        Console.WriteLine("limit " + Join(comma.Split("a,b,c,d", 2)) + " " + Join(comma.Split("a,b,c,d", 3)) + " " + Join(comma.Split("a,b,c,d", 1)) + " " + Join(comma.Split("a,b,c,d", 9)) + " " + Join(comma.Split("a,b,c,d", 0)));
        Console.WriteLine("splitat " + Join(comma.Split("a,b,c,d", 0, 2)));
        Console.WriteLine("options " + Join(Regex.Split("aXbxc", "x", RegexOptions.IgnoreCase)));

        // Replace with an evaluator: it is called with each match, in order.
        Console.WriteLine("evaluator " + Regex.Replace("a1b22c", "\\d+", m => "<" + m.Value.Length + ">") + " " + Regex.Replace("a1b22c", "\\d+", m => m.Value + m.Value));
        Int32[] calls = new Int32[1];
        Regex word = new Regex("\\w+");
        Console.WriteLine("evaluator2 " + word.Replace("one two three", m => { calls[0]++; return m.Value.ToUpper() + calls[0]; }) + " " + calls[0]);
        Console.WriteLine("evaluator3 " + word.Replace("one two three", m => "-", 2) + " " + word.Replace("one two three", m => "-", -1, 4) + " " + word.Replace("one two", m => "-", 0));
        Console.WriteLine("evaluator4 " + Regex.Replace("Hello World", "[a-z]+", m => m.Value.ToUpper(), RegexOptions.None) + " " + Regex.Replace("ab", "(?<x>a)", m => m.Groups["x"].Value + m.Groups[1].Index) + " " + new Regex("").Replace("ab", m => "|"));

        // Escape and Unescape.
        String text = "a.b*c (d)[e]{f}|g^h$i+j?k\\l#m n\to\np";
        String escaped = Regex.Escape(text);
        Console.WriteLine("escape " + escaped);
        Console.WriteLine("formfeed " + Regex.Escape("a" + (Char)12 + "f") + " " + Regex.Unescape("\\f\\v").Length + " " + new Regex("a\\fb").IsMatch("a" + (Char)12 + "b"));
        Console.WriteLine("escape2 " + Regex.Escape("1+1=2") + " " + Regex.Escape("plain") + " [" + Regex.Escape("") + "]");
        Console.WriteLine("unescape " + Regex.Unescape(escaped).Equals(text) + " " + Regex.Unescape("a\\.b\\n").Length + " " + Regex.Unescape("plain"));
        Console.WriteLine("roundtrip " + new Regex(escaped).IsMatch(text) + " " + new Regex(Regex.Escape("a.b")).IsMatch("axb") + " " + new Regex(Regex.Escape("a.b")).IsMatch("a.b"));

        // The static methods keep the patterns that were used last: more than 15 patterns still work.
        Int32 found = 0;
        for (Int32 round = 0; round < 2; round++)
        {
            for (Int32 i = 0; i < 20; i++)
            {
                if (Regex.IsMatch("x" + i, "x" + i + "$"))
                    found++;
            }
        }

        Console.WriteLine("cache " + found + " " + Regex.IsMatch("A", "a") + " " + Regex.IsMatch("A", "a", RegexOptions.IgnoreCase) + " " + Regex.IsMatch("A", "a"));
        Console.WriteLine("match " + Regex.Match("a12", "\\d+").Value + " " + Regex.Match("a12", "\\D+", RegexOptions.None).Value + " " + Regex.Matches("a12", "\\d", RegexOptions.None).Count);
        Console.WriteLine("string " + new Regex("a+b", RegexOptions.IgnoreCase).ToString() + " " + (Int32)new Regex("x").Options + " " + (Int32)new Regex("x", RegexOptions.IgnoreCase | RegexOptions.Multiline).Options);

        Console.WriteLine("done");
    }
}
