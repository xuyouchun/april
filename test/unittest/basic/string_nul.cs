// The length of a string is kept with it: a string can have the character 0, a literal with \u0000
// and a string that the program makes.

/* EXPECT

3
4 0 2 3
3 0 x
True False
2 3 1
3 0 b
3 True False
done

*/

import System;

using System;
using System.Text;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        String s = String.FromChars(new Char[] { 'a', '\0', 'b' });
        Console.WriteLine(s.Length);

        String t = s + "c";
        Console.WriteLine(t.Length + " " + (Int32)t[1] + " " + t.IndexOf('b') + " " + t.IndexOf("c"));

        StringBuilder sb = new StringBuilder();
        sb.Length = 2;
        sb.Append('x');
        String u = sb.ToString();
        Console.WriteLine(u.Length + " " + (Int32)u[0] + " " + u[2]);

        String same = new StringBuilder().Append('a').Append('\0').Append('b').ToString();
        Console.WriteLine((s == same) + " " + (s == "a"));
        Console.WriteLine(s.Substring(1, 2).Length + " " + s.Trim().Length + " " + s.LastIndexOf('\0'));
        String lit = "a\u0000b";
        Console.WriteLine(lit.Length + " " + (Int32)lit[1] + " " + lit[2]);

        String folded = "x" + "\u0000" + "y";
        Console.WriteLine(folded.Length + " " + (lit == same) + " " + (lit == "a"));
        Console.WriteLine("done");
    }
}
