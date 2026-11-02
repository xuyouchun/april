// EXPECT_WARNING 4006 4006

/* EXPECT

*/

#nullable enable

import System;

using System;
using System.Collections.Generic;

class Program
{
    // Never called.
    static void Cases(Dictionary<String, String> d, Object o, String? maybe)
    {
        // The pattern: the object is not null, the variable is.
        if (o is String s)
            Console.WriteLine(s.Length);                // ok

        // out var: it can be null, unless the call returns true.
        if (d.TryGetValue("a", out var v))
            Console.WriteLine(v.Length);                // ok

        d.TryGetValue("b", out var w);
        Console.WriteLine(w.Length);                    // 4006: the call can return false

        if (d.TryGetValue("c", out String? x))
            Console.WriteLine(x.Length);                // ok
        else
            Console.WriteLine(x.Length);                // 4006: it is null when the call returns false

        if (maybe is String t)
            Console.WriteLine(t.Length);                // ok
    }

    [EntryPoint]
    public static void Main()
    {
    }
}
