// EXPECT_WARNING 4005

/* EXPECT

3
ok

*/

import System;

using System;

// No directive: the references can be null, the types with ? are accepted.
class Node
{
    public String? Name;
    public Node Next = null;
}

class Program
{
    static int Length(String s)
    {
        return s.Length;
    }

    [EntryPoint]
    public static void Main()
    {
        String s = null;
        Node n = null;
        Console.WriteLine(Length("abc"));

#nullable enable
        // Enabled from here until disable: the warning is the 4005 of the next line.
        String t = null;
#nullable disable
        String u = null;

        Console.WriteLine(s == null && n == null && t == null && u == null? "ok" : "no");
    }
}
