// EXPECT_WARNING 4006 4006

/* EXPECT

3
0
2
0
-1

*/

#nullable enable

import System;

using System;

class Node
{
    public String[]? Items;
}

class Program
{
    static int NotNull(String? s)
    {
        if (s is not null)
            return s.Length;

        return 0;
    }

    static int IsNull(String? s)
    {
        if (s is null)
            return 0;

        return s.Length;
    }

    // The object of ?[ and ?. is not dereferenced.
    static int Index(String[]? a)
    {
        return a?[0]?.Length ?? -1;
    }

    static int Member(Node? n)
    {
        return n?.Items?[0]?.Length ?? 0;
    }

    // Never called: the dereference of a value that can be null.
    static int Wrong(Node? n)
    {
        return n.Items[0].Length;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(NotNull("abc"));
        Console.WriteLine(IsNull(null));

        String[] a = new String[1];
        a[0] = "xy";
        Console.WriteLine(Index(a));

        Node n = new Node();
        Console.WriteLine(Member(n));
        Console.WriteLine(Index(null));
    }
}
