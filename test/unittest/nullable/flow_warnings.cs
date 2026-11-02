// EXPECT_WARNING 4005 4005 4004 4006 4006 4005 4007 4006 4008

/* EXPECT

*/

#nullable enable

import System;

using System;

class Node
{
    public String? Name;
    public String Title = "t";
    public Node? Next;

    public String? Find() { return Name; }
}

class Program
{
    static void Take(String s) { }

    static void TakeOptional(String? s) { }

    static String Wrong() { return null; }

    // Never called.
    static void Cases(Node n, String? s)
    {
        String a = null;                    // 4005: the null literal
        String b = s;                       // 4004: a value that can be null
        Console.WriteLine(s.Length);        // 4006: dereference
        Console.WriteLine(n.Name.Length);   // 4006: dereference of a member that can be null
        Take(null);                         // 4005: the null literal as an argument
    }

    static String Return(String? s)
    {
        return s;                           // 4007: return
    }

    static void Other(Node n, String? s)
    {
        var f = n.Find();
        Console.WriteLine(f.Length);        // 4006: var can be null
        Take(s);                            // 4008: an argument
    }

    [EntryPoint]
    public static void Main()
    {
    }
}
