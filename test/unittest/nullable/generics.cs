// EXPECT_WARNING 4006 4006 4008 4005 4006 4006

/* EXPECT

*/

#nullable enable

import System;
using System;
using System.Collections.Generic;

class Program
{
    static void Take(String s) { }

    // Never called.
    static void Lists()
    {
        List<String?> maybe = new List<String?>();
        List<String> plain = new List<String>();
        Stack<String?> stack = new Stack<String?>();
        Stack<String> plain_stack = new Stack<String>();

        maybe.Add(null);                            // an argument that can be null: no warning
        plain.Add("x");

        Console.WriteLine(plain[0].Length);         // not null: no warning
        Console.WriteLine(plain_stack.Peek().Length);   // not null: no warning
        Console.WriteLine(maybe[0].Length);         // 4006: the argument can be null
        Console.WriteLine(stack.Peek().Length);     // 4006: a method too
        Take(stack.Peek());                         // 4008
        plain_stack.Push(null);                     // 4005: it cannot be null
    }

    // The out params.
    static int Outs(Dictionary<String, String> d, Dictionary<String, String?> e)
    {
        String? v;
        if (d.TryGetValue("a", out v))
            return v.Length;                        // no warning: not null when it is true

        String? w;
        d.TryGetValue("b", out w);
        Console.WriteLine(w.Length);                // 4006: it can be null when it is false

        var f = new Dictionary<String, String?>();
        String? x;
        if (f.TryGetValue("c", out x))
            Console.WriteLine(x.Length);            // 4006: the value can be null

        return 0;
    }

    [EntryPoint]
    public static void Main()
    {
    }
}
