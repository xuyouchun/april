// The blocks of a method that follow each other: the temporary variables of a statement are defined in the region of
// its own block, not in the one of the last block of the method.

/* EXPECT

infinite -10000
True

*/

import System;
using System;

struct T { public Int64 V; public T(Int64 v) { V = v; } public static T Make(Int64 v) { return new T(v); } }

class Rx
{
    public Rx(String p) : this(p, 0, new T(-10000)) { }
    public Rx(String p, Int32 o, T t) : this(p, o, 0, t) { }
    private Rx(String p, Int32 o, Int64 m, T t) { Pattern = p; timeout = t; }
    public String Pattern;
    private T timeout;
    public T Timeout => timeout;
    public Boolean IsMatch(String s) { return s.Length > 0; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        {
            String text = "aaac";
            Console.WriteLine("infinite " + new Rx("a").Timeout.V);
            Console.WriteLine(new Rx("a", 0, T.Make(500)).IsMatch(text));
        }
        {
            {
            }
        }
    }
}
