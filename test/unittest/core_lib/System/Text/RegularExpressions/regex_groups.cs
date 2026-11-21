// Regex: the groups of a match: numbers, names, captures of a group in a loop, groups that did not match, the
// next match, a search that starts at an index.

/* EXPECT

numbers 3 12-345 12 345
index 4 2 7 True
named 3 2026 11 year 11
names 3:0,year,month 2 year -1 []
mixed 4:0,1,2,n bca a
quote a
same 2:0,a y x
explicit 2 b 2:0,x
noncapture 2 2
failed False [] 0 True b
outside False False []
undone ab False
backtrack a|bcd||True
loop c 3 a@0 b@2 c@4
digits 3@3 3 1@1 2@2 3@3
kept a 1 a@0
optional False 3
whole 1 a 1
next 1@1 22@3 333@6 False []
empty 4 []@0[x]@1[]@2[]@3
foreach 6
collection 0 2
startat 2@3 False True False
anchor False True True
startat out of range
result 1-a-a=1-$ a=1 1
empty match False 0 0 False
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    static String Join(String[] parts)
    {
        return parts.Length + ":" + String.Join(",", parts);
    }

    static String Caps(Group g)
    {
        String s = g.Captures.Count + "";
        for (Int32 i = 0; i < g.Captures.Count; i++)
            s += " " + g.Captures[i].Value + "@" + g.Captures[i].Index;

        return s;
    }

    [EntryPoint]
    public static void Main()
    {
        // Numbers.
        {
            Match m = new Regex("(\\d+)-(\\d+)").Match("tel 12-345 x");
            Console.WriteLine("numbers " + m.Groups.Count + " " + m.Groups[0].Value + " " + m.Groups[1].Value + " " + m.Groups[2].Value);
            Console.WriteLine("index " + m.Groups[1].Index + " " + m.Groups[1].Length + " " + m.Groups[2].Index + " " + m.Groups[2].Success);
        }

        // Names: the groups that have no name are numbered first.
        {
            Regex r = new Regex("(?<year>\\d{4})-(?<month>\\d{2})");
            Match m = r.Match("on 2026-11-21");
            Console.WriteLine("named " + m.Groups.Count + " " + m.Groups["year"].Value + " " + m.Groups["month"].Value + " " + m.Groups[1].Name + " " + m.Groups[2].Value);
            Console.WriteLine("names " + Join(r.GetGroupNames()) + " " + r.GroupNumberFromName("month") + " " + r.GroupNameFromNumber(1) + " " + r.GroupNumberFromName("none") + " [" + r.GroupNameFromNumber(7) + "]");

            Regex r2 = new Regex("(?<n>a)(b)(c)");
            Match m2 = r2.Match("abc");
            Console.WriteLine("mixed " + Join(r2.GetGroupNames()) + " " + m2.Groups[1].Value + m2.Groups[2].Value + m2.Groups[3].Value + " " + m2.Groups["n"].Value);

            Regex r3 = new Regex("(?'q'a)\\w");
            Console.WriteLine("quote " + r3.Match("ab").Groups["q"].Value);

            Regex r4 = new Regex("(?<a>x)|(?<a>y)");
            Console.WriteLine("same " + Join(r4.GetGroupNames()) + " " + r4.Match("y").Groups["a"].Value + " " + r4.Match("x").Groups["a"].Value);
        }

        // ExplicitCapture, (?:...) and (?n).
        {
            Regex r = new Regex("(a)(?<x>b)", RegexOptions.ExplicitCapture);
            Match m = r.Match("ab");
            Console.WriteLine("explicit " + m.Groups.Count + " " + m.Groups[1].Value + " " + Join(r.GetGroupNames()));
            Console.WriteLine("noncapture " + new Regex("(?:a)(b)").Match("ab").Groups.Count + " " + new Regex("(?n)(a)(?<x>b)").Match("ab").Groups.Count);
        }

        // Groups that did not match.
        {
            Match m = new Regex("(a)|(b)").Match("b");
            Console.WriteLine("failed " + m.Groups[1].Success + " [" + m.Groups[1].Value + "] " + m.Groups[1].Captures.Count + " " + m.Groups[2].Success + " " + m.Groups[2].Value);
            Console.WriteLine("outside " + m.Groups[9].Success + " " + m.Groups["nope"].Success + " [" + m.Groups[9].Value + "]");

            Match n = new Regex("(a)x|ab").Match("ab");
            Console.WriteLine("undone " + n.Value + " " + n.Groups[1].Success);

            Match o = new Regex("(a|ab)(c|bcd)(d*)").Match("abcd");
            Console.WriteLine("backtrack " + o.Groups[1].Value + "|" + o.Groups[2].Value + "|" + o.Groups[3].Value + "|" + o.Groups[3].Success);
        }

        // The captures of a group in a loop.
        {
            Match m = new Regex("(?:(\\w)\\s?)+").Match("a b c");
            Console.WriteLine("loop " + m.Groups[1].Value + " " + Caps(m.Groups[1]));

            Match n = new Regex("(\\d)+").Match("x123y");
            Console.WriteLine("digits " + n.Groups[1].Value + "@" + n.Groups[1].Index + " " + Caps(n.Groups[1]));

            Match o = new Regex("(?:(a)|b)*c").Match("abc");
            Console.WriteLine("kept " + o.Groups[1].Value + " " + Caps(o.Groups[1]));

            Match p = new Regex("(a)(b)?").Match("a");
            Console.WriteLine("optional " + p.Groups[2].Success + " " + p.Groups.Count);
            Console.WriteLine("whole " + p.Captures.Count + " " + p.Captures[0].Value + " " + p.Groups[0].Captures.Count);
        }

        // The next match.
        {
            Match m = new Regex("\\d+").Match("a1b22c333");
            String s = "";
            while (m.Success)
            {
                s += m.Value + "@" + m.Index + " ";
                m = m.NextMatch();
            }

            Console.WriteLine("next " + s + m.Success + " [" + m.Value + "]");

            MatchCollection all = new Regex("x*").Matches("axb");
            s = "";
            for (Int32 i = 0; i < all.Count; i++)
                s += "[" + all[i].Value + "]@" + all[i].Index;

            Console.WriteLine("empty " + all.Count + " " + s);

            Int32 total = 0;
            foreach (Match each in new Regex("\\w").Matches("a b c"))
                total += each.Index;

            Console.WriteLine("foreach " + total);
            Console.WriteLine("collection " + new Regex("z").Matches("abc").Count + " " + Regex.Matches("a1b2", "\\d").Count);
        }

        // A search from an index: ^ is the beginning of the text, not the index.
        {
            Regex r = new Regex("\\d");
            Match m = r.Match("a1b2", 2);
            Console.WriteLine("startat " + m.Value + "@" + m.Index + " " + r.IsMatch("a1b", 2) + " " + r.IsMatch("a1b", 1) + " " + r.IsMatch("a1b", 3));
            Console.WriteLine("anchor " + new Regex("^a").Match("aa", 1).Success + " " + new Regex("^a").Match("aa", 0).Success + " " + new Regex("\\Ba").Match("aa", 1).Success);

            try
            {
                r.Match("abc", 4);
                Console.WriteLine("no error");
            }
            catch (OutOfRangeException)
            {
                Console.WriteLine("startat out of range");
            }
        }

        // The result of a match with the substitutions.
        {
            Match m = new Regex("(?<k>\\w+)=(?<v>\\w+)").Match("a=1");
            Console.WriteLine("result " + m.Result("$2-${k}-$&-$$") + " " + m.ToString() + " " + m.Groups[2].ToString());
        }

        Console.WriteLine("empty match " + Match.Empty.Success + " " + Match.Empty.Value.Length + " " + Match.Empty.Groups.Count + " " + Match.Empty.NextMatch().Success);

        Console.WriteLine("done");
    }
}
