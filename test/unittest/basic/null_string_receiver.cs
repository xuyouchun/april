// A string that is null is the empty string for its methods (the interpreter, the JIT and the native programs agree).

/* EXPECT

length 0 0
methods True True 0 -1 [] []
concat [] [x] [x]
format [] [a-]
compare True False

*/

import System;

using System;

class Program
{
    static String Nothing() => null;

    [EntryPoint]
    public static void Main()
    {
        String s = null;
        Console.WriteLine("length " + s.Length + " " + Nothing().Length);

        Console.WriteLine("methods " + s.StartsWith("") + " " + s.EndsWith("") + " " + s.Length + " " + s.IndexOf("a")
            + " [" + s.ToUpper() + "] [" + s.Trim() + "]");

        String x = "x";
        Console.WriteLine("concat [" + s + "] [" + s + x + "] [" + x + s + "]");

        Console.WriteLine("format [" + String.Format("{0}", s) + "] [" + String.Format("{0}-{1}", "a", s) + "]");

        Console.WriteLine("compare " + (s == null) + " " + (s == "x"));
    }
}
