// Regex: the backtracking is kept in an array, not in the calls: long texts, loops of many turns, loops that can
// match nothing, counted loops that are made by copies, lazy loops.

/* EXPECT

long True 100001
nothing False
alternation True 40001
captures 20000 b b a
lazy <a> <a><b> 3
least aaab aaa a
lazy count aa aaaab ababc
count aaaaaa aaaaab True False
count captures 3 3 False
nested abcbac 8
empty loops aaa [] aaa aab aab
empty lazy aab aab x
atomic 60001 False 60000 60000 True
limit 100000 (a+)+b 31 100000
atomic fast False True 0
alternatives True True False False
groups 51 115
first 90000 False False
matches 5000 3001
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

    [EntryPoint]
    public static void Main()
    {
        // A long text: the stack of the choices grows, there is no recursion.
        {
            String text = Repeat("a", 100000) + "b";
            Match m = new Regex("a*b").Match(text);
            Console.WriteLine("long " + m.Success + " " + m.Length);

            Match n = new Regex("^.*x").Match(Repeat("y", 50000));
            Console.WriteLine("nothing " + n.Success);

            Match o = new Regex("^(?:a|b)*c$").Match(Repeat("ab", 20000) + "c");
            Console.WriteLine("alternation " + o.Success + " " + o.Length);
        }

        // The captures of a loop of many turns.
        {
            Match m = new Regex("(a|b)*c").Match(Repeat("ab", 10000) + "c");
            Console.WriteLine("captures " + m.Groups[1].Captures.Count + " " + m.Groups[1].Value + " " + m.Groups[1].Captures[19999].Value + " " + m.Groups[1].Captures[0].Value);
        }

        // Greedy, lazy, and the least that makes the whole match.
        Console.WriteLine("lazy " + new Regex("<.+?>").Match("<a><b>").Value + " " + new Regex("<.+>").Match("<a><b>").Value + " " + new Regex("<.+?>").Matches("<a><b><c>").Count);
        Console.WriteLine("least " + new Regex("a*?b").Match("aaab").Value + " " + new Regex("a+?$").Match("aaa").Value + " " + new Regex("(a+?)(a*)").Match("aaa").Groups[1].Value);
        Console.WriteLine("lazy count " + new Regex("a{2,4}?").Match("aaaaa").Value + " " + new Regex("a{2,4}?b").Match("aaaaab").Value + " " + new Regex("(?:ab){1,2}?c").Match("ababc").Value);

        // Loops of a count are made by copies of the part.
        Console.WriteLine("count " + new Regex("(a{2}){3}").Match("aaaaaaaa").Value + " " + new Regex("a{3,5}b").Match("aaaaaab").Value + " " + new Regex("^(?:ab){1000}$").IsMatch(Repeat("ab", 1000)) + " " + new Regex("^(?:ab){1000}$").IsMatch(Repeat("ab", 999)));
        Console.WriteLine("count captures " + new Regex("(ab){3}").Match("ababab").Groups[1].Captures.Count + " " + new Regex("(a){2,3}").Match("aaaa").Groups[1].Captures.Count + " " + new Regex("(a){0}b").Match("b").Groups[1].Success);
        Console.WriteLine("nested " + new Regex("((a|b){2}c){2}").Match("abcbac").Value + " " + new Regex("(((a){2}){2}){2}").Match("aaaaaaaaa").Value.Length);

        // Loops that can match nothing stop when a turn matches nothing.
        Console.WriteLine("empty loops " + new Regex("(a*)*").Match("aaa").Value + " [" + new Regex("(a*)*").Match("b").Value + "] " + new Regex("(a*)+").Match("aaab").Value + " " + new Regex("(a|)*b").Match("aab").Value + " " + new Regex("(?:|a)+b").Match("aab").Value);
        Console.WriteLine("empty lazy " + new Regex("(a*?)*?b").Match("aab").Value + " " + new Regex("(?:a?)*?b").Match("aab").Value + " " + new Regex("(?:\\b)*x").Match("x").Value);

        // Atomic groups and assertions on a long text: the stack of the choices is cut, and it does not grow.
        {
            String text = Repeat("a", 60000) + "b";
            Console.WriteLine("atomic " + new Regex("(?>a+)b").Match(text).Length + " " + new Regex("^(?>a+)ab").IsMatch(text) + " " + new Regex("^(?=a+b)a+").Match(text).Length + " " + new Regex("(?<=a)b").Match(text).Index + " " + new Regex("^(?:(?>a)|b)+$").IsMatch(text));
        }

        // The limit of the steps of a search (the library has no clock): a pattern that takes the time of 2 to the
        // power of the length of the text.
        {
            String text = Repeat("a", 30) + "c";
            Regex limited = new Regex("(a+)+b", RegexOptions.None, 100000);
            try
            {
                limited.IsMatch(text);
                Console.WriteLine("no limit");
            }
            catch (RegexMatchTimeoutException e)
            {
                Console.WriteLine("limit " + e.MaxSteps + " " + e.Pattern + " " + (e.Input.Length) + " " + limited.MaxSteps);
            }

            // The atomic group has no such cost, and a search that is short is not stopped.
            Console.WriteLine("atomic fast " + new Regex("(?>a+)+b", RegexOptions.None, 100000).IsMatch(text) + " " + new Regex("a+b", RegexOptions.None, 100).IsMatch("aab") + " " + new Regex("a+b").MaxSteps);
        }

        // Many alternatives, many groups.
        {
            StringBuilder sb = new StringBuilder();
            for (Int32 i = 0; i < 200; i++)
            {
                if (i > 0)
                    sb.Append("|");

                sb.Append("word" + i);
            }

            Regex r = new Regex("^(?:" + sb.ToString() + ")$");
            Console.WriteLine("alternatives " + r.IsMatch("word0") + " " + r.IsMatch("word199") + " " + r.IsMatch("word200") + " " + r.IsMatch("word"));

            StringBuilder groups = new StringBuilder();
            for (Int32 i = 0; i < 50; i++)
                groups.Append("(\\d)");

            Match m = new Regex(groups.ToString()).Match(Repeat("12345", 10));
            Console.WriteLine("groups " + m.Groups.Count + " " + m.Groups[1].Value + m.Groups[6].Value + m.Groups[50].Value);
        }

        // The search of the first character, and an anchored pattern.
        Console.WriteLine("first " + new Regex("needle").Match(Repeat("hay", 30000) + "needle").Index + " " + new Regex("^needle").Match(Repeat("hay", 30000) + "needle").Success + " " + new Regex("\\Aa").Match("ba").Success);

        // The matches of a long text.
        Console.WriteLine("matches " + new Regex("\\d+").Matches(Repeat("ab12 ", 5000)).Count + " " + new Regex("x*").Matches(Repeat("y", 3000)).Count);

        Console.WriteLine("done");
    }
}
