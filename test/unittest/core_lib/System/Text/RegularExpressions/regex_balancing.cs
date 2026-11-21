// Regex: the balancing groups (?<a-b>x) and (?<-b>x): the group cancels the last capture of b, and a gets the text between
// the two; the captures that are cancelled are not in the result and are not seen by a reference or a condition.

/* EXPECT

^(?:[^()]|(?<open>\()|(?<-open>\)))*(?(open)(?!))$ | (a(b)c) | [(a(b)c)]@0
^(?:[^()]|(?<open>\()|(?<-open>\)))*(?(open)(?!))$ | (a(b)c | -
^(?:[^()]|(?<open>\()|(?<-open>\)))*(?(open)(?!))$ | a)b( | -
^(?:[^()]|(?<open>\()|(?<-open>\)))*(?(open)(?!))$ | ((()))() | [((()))()]@0
^(?:[^()]|(?<open>\()|(?<-open>\)))*(?(open)(?!))$ |  | []@0
^(?:[^()]|(?<open>\()|(?<-open>\)))*(?(open)(?!))$ | (() | -
^(?:[^()]|(?<open>\()|(?<-open>\)))*(?(open)(?!))$ | ()) | -
adjacent False 0 | 1 []@1
between x_y 0 | 1 [_]@1
inside 1 []@1
pairs True 0 | 2 [b]@3 [a<b>c]@1
nothing False True False
(?<x>a)(?<y>b)(?<-y>c)\k<y> | abcb | -
(?<x>a)(?<y>b)(?<y>c)(?<-y>d)\k<y> | abcdb | [abcdb]@0
(?<x>a)(?<-x>b)(?(x)c|d) | abd | [abd]@0
(?<x>a)(?<-x>b)(?(x)c|d) | abc | -
(?<x>a)(?<x>b)(?<-x>c)(?(x)d|e) | abcd | [abcd]@0
popped True 1 [a]@0 a
undone ab 1 [a]@0
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    static void M(String pattern, String text)
    {
        Match m = new Regex(pattern).Match(text);
        Console.WriteLine(pattern + " | " + text + " | " + (m.Success ? "[" + m.Value + "]@" + m.Index : "-"));
    }

    static String Caps(Group g)
    {
        String s = g.Captures.Count + "";
        for (Int32 i = 0; i < g.Captures.Count; i++)
            s += " [" + g.Captures[i].Value + "]@" + g.Captures[i].Index;

        return s;
    }

    [EntryPoint]
    public static void Main()
    {
        // The parentheses that are balanced.
        String balanced = "^(?:[^()]|(?<open>\\()|(?<-open>\\)))*(?(open)(?!))$";
        M(balanced, "(a(b)c)");
        M(balanced, "(a(b)c");
        M(balanced, "a)b(");
        M(balanced, "((()))()");
        M(balanced, "");
        M(balanced, "(()");
        M(balanced, "())");

        // The text between the two groups.
        Match m = new Regex("(?<open>a)(?<close-open>b)").Match("ab");
        Console.WriteLine("adjacent " + m.Groups["open"].Success + " " + Caps(m.Groups["open"]) + " | " + Caps(m.Groups["close"]));

        Match n = new Regex("(?<a>x).(?<b-a>y)").Match("x_y");
        Console.WriteLine("between " + n.Value + " " + Caps(n.Groups["a"]) + " | " + Caps(n.Groups["b"]));

        Match o = new Regex("(?<a>x)(?<b-a>.{2})").Match("xyz");
        Console.WriteLine("inside " + Caps(o.Groups["b"]));

        // Nested: the pairs are matched one with the other.
        Match p = new Regex("^(?:(?<o><)|[a-z]|(?<c-o>>))*$").Match("<a<b>c>");
        Console.WriteLine("pairs " + p.Success + " " + Caps(p.Groups["o"]) + " | " + Caps(p.Groups["c"]));

        // A group that cancels but does not capture, and one that has nothing to cancel.
        Console.WriteLine("nothing " + new Regex("(?<-x>a)(?<x>b)").IsMatch("ab") + " " + new Regex("(?<x>b)(?<-x>a)").IsMatch("ba") + " " + new Regex("(?<x>b)(?<-x>a)(?<-x>c)").IsMatch("bac"));

        // The reference to a group, and the condition, see the captures that are not cancelled.
        M("(?<x>a)(?<y>b)(?<-y>c)\\k<y>", "abcb");
        M("(?<x>a)(?<y>b)(?<y>c)(?<-y>d)\\k<y>", "abcdb");
        M("(?<x>a)(?<-x>b)(?(x)c|d)", "abd");
        M("(?<x>a)(?<-x>b)(?(x)c|d)", "abc");
        M("(?<x>a)(?<x>b)(?<-x>c)(?(x)d|e)", "abcd");

        // The captures of the group that is balanced.
        Match q = new Regex("(?<x>a)(?<x>b)(?<-x>c)").Match("abc");
        Console.WriteLine("popped " + q.Groups["x"].Success + " " + Caps(q.Groups["x"]) + " " + q.Groups["x"].Value);

        // The cancel is undone when the run goes back.
        Match r = new Regex("(?<x>a)(?:(?<-x>b)c|b)").Match("abd");
        Console.WriteLine("undone " + r.Value + " " + Caps(r.Groups["x"]));

        Console.WriteLine("done");
    }
}
