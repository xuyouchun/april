// StringBuilder: appending, inserting, removing, replacing, the indexer, the length and the capacity.

/* EXPECT

append abc12True2.5o1 14
more x---pq 6
lines 2
edit >ello, world 12
replace >eLL0, w0rLd
indexer <e LL0
cut <eL 3
fill 5 0
big 2000 True abab
string 007 7** a-b 中x 2
generic 37-1.5True5/ 12
range caught
more2 abcdef a,b,c ab-ab-ab 3 True False
range2 a-x->-c a>>c xyz 5
join self abab 4

*/

import System;

using System;
using System.Text;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        StringBuilder sb = new StringBuilder();
        sb.Append("ab").Append('c').Append(12).Append(true).Append(2.5).Append((Object)"o").Append(1L);
        Console.WriteLine("append " + sb.ToString() + " " + sb.Length);

        sb.Clear().Append("x", 0, 1).Append('-', 3).Append(new Char[] { 'p', 'q' });
        Console.WriteLine("more " + sb + " " + sb.Length);

        sb.AppendLine("line").Append("end");
        Console.WriteLine("lines " + sb.ToString().Split('\n').Length);

        StringBuilder t = new StringBuilder("hello world");
        t.Insert(5, ",").Insert(0, '>').Remove(1, 1);
        Console.WriteLine("edit " + t + " " + t.Length);

        t.Replace("o", "0").Replace('l', 'L');
        Console.WriteLine("replace " + t);

        t[0] = '<';
        Console.WriteLine("indexer " + t[0] + t[1] + " " + t.ToString(2, 3));

        t.Length = 3;
        Console.WriteLine("cut " + t + " " + t.Length);

        t.Length = 5;
        Console.WriteLine("fill " + t.Length + " " + (Int32)t[4]);

        StringBuilder big = new StringBuilder(1);
        for (Int32 k = 0; k < 1000; k++)
            big.Append("ab");

        Console.WriteLine("big " + big.Length + " " + (big.Capacity >= 2000) + " " + big.ToString(1996, 4));

        Console.WriteLine("string " + "7".PadLeft(3, '0') + " " + "7".PadRight(3, '*')
            + " " + String.Join("-", new String[] { "a", "b" }) + " " + String.FromChars(new Char[] { '中', 'x' })
            + " " + "😀".ToCharArray().Length);

        StringBuilder g = new StringBuilder();
        g.Append(7).Append(-1.5).Append(true).AppendLine(5L).Insert(0, (UInt32)3);
        Console.WriteLine("generic " + g.ToString().Replace("\n", "/") + " " + g.Length);

        try
        {
            t.Remove(4, 9);
            Console.WriteLine("no");
        }
        catch (OutOfRangeException)
        {
            Console.WriteLine("range caught");
        }

        StringBuilder u = new StringBuilder("abc");
        u.Append(new StringBuilder("de")).Append(new Char[] { 'x', 'f', 'y' }, 1, 1);
        StringBuilder v = new StringBuilder().AppendJoin(",", new String[] { "a", "b", "c" });
        StringBuilder w = new StringBuilder("ab").Insert(2, "-ab", 2).Insert(0, new Char[] { 'a', 'b' });
        Console.WriteLine("more2 " + u + " " + v + " " + w.ToString(2, 8) + " " + w.Length / 3
            + " " + new StringBuilder("q").Equals(new StringBuilder("q")) + " " + u.Equals(v));

        StringBuilder r = new StringBuilder("a-b-c");
        r.Replace("-", "-x-", 1, 1).Replace('-', '-', 0, 0).Replace('b', '>', 4, 1);
        StringBuilder s2 = new StringBuilder("a>>c").Replace('>', '>', 1, 2);
        Char[] dst = new Char[5];
        new StringBuilder("xyz").CopyTo(0, dst, 1, 3);
        Console.WriteLine("range2 " + r + " " + s2 + " " + String.FromChars(dst).Substring(1, 3) + " " + dst.Length);

        StringBuilder self = new StringBuilder("ab");
        self.Append(self);
        Console.WriteLine("join self " + self + " " + self.Length);
    }
}
