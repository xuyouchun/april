// The strings that a program makes are garbage collected (they are atomic spaces of the heap: the collector
// never scans them, and a pointer to their characters keeps them): the ones that are held keep their
// characters while a lot of others are made and dropped, the strings of arrays, lists, fields and the
// results of Substring / Replace / string.Format are all reachable the same way.

/* EXPECT

kept 400 bad 0
total 6633335
fields ok
big 10485750 ok

*/

import System;

using System;
using System.Collections;

class Holder
{
    public String First;
    public String[] Many;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        List<String> kept = new List<String>();
        Int64 total = 0;

        for (Int32 i = 0; i < 400000; i++)
        {
            String s = "item" + i + "-" + (i * 2);
            total += s.Length;

            if (i % 1000 == 0)
                kept.Add(s);
        }

        Int32 bad = 0;
        for (Int32 k = 0; k < kept.Count; k++)
        {
            Int32 i = k * 1000;
            if (!kept[k].Equals("item" + i + "-" + (i * 2)))
                bad++;
        }

        Console.WriteLine("kept " + kept.Count + " bad " + bad);
        Console.WriteLine("total " + total);

        // A holder that keeps strings in a field and in an array while the garbage is made.
        Holder h = new Holder();
        h.First = "first-" + 12345;
        h.Many = new String[100];
        for (Int32 i = 0; i < 100; i++)
            h.Many[i] = ("many-" + i).Substring(2).Replace("n", "N");

        for (Int32 i = 0; i < 300000; i++)
        {
            String t = "garbage" + i;
            total += t.Length;
        }

        Boolean ok = h.First.Equals("first-12345");
        for (Int32 i = 0; i < 100; i++)
            ok = ok && h.Many[i].Equals("Ny-" + i);

        Console.WriteLine("fields " + (ok? "ok" : "BAD"));

        // A string that is big (a large object of the heap).
        String big = "";
        for (Int32 i = 0; i < 20; i++)
            big = big + big + "abcdefghij";

        Int32 pieces = 0;
        for (Int32 i = 0; i < 200; i++)
        {
            String block = "x" + i;
            pieces += block.Length;
        }

        Console.WriteLine("big " + big.Length + " " + (big.Substring(0, 10).Equals("abcdefghij")? "ok" : "BAD"));
    }
}
