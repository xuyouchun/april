// EXPECT_WARNING none

/* EXPECT

7
5
none
hi
5
2
1
x
b
done

*/

#nullable enable

import System;

using System;

class Node
{
    public String? Name;
    public Node? Next;
    public int N;

    public String? Find(int k) { return k > 0? "x" : null; }
}

class Program
{
    static int Len(String? s)
    {
        // A check before: s is not null in the branch.
        if (s != null)
            return s.Length;

        return -1;
    }

    static int EarlyReturn(String? s)
    {
        if (s == null)
            return 0;

        return s.Length;
    }

    static String Coalesce(String? s)
    {
        return s ?? "none";
    }

    static String Forgiving(String? s)
    {
        return s!;
    }

    static int Both(String? a, String? b)
    {
        if (a != null && b != null)
            return a.Length + b.Length;

        return 0;
    }

    static int Either(String? a, String? b)
    {
        if (a == null || b == null)
            return 0;

        return a.Length + b.Length;
    }

    static int Loop(Node? n)
    {
        int count = 0;
        while (n != null)
        {
            count++;
            n = n.Next;
        }

        return count;
    }

    static String Assigned(String? s)
    {
        s = "hi";
        return s;
    }

    static int Ternary(String? s)
    {
        return s != null? s.Length : 0;
    }

    static String Conditional(Node? n)
    {
        return n?.Name ?? "b";
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Len("abcdefg"));
        Console.WriteLine(EarlyReturn("12345"));
        Console.WriteLine(Coalesce(null));
        Console.WriteLine(Assigned(null));
        Console.WriteLine(Both("ab", "cde"));

        Node first = new Node();
        first.Next = new Node();
        Console.WriteLine(Loop(first));
        Console.WriteLine(Ternary("x"));

        Node n = new Node();
        String? f = n.Find(1);
        if (f != null)
            Console.WriteLine(f);

        Console.WriteLine(Conditional(null));
        Console.WriteLine(Forgiving("ok") == "ok"? "done" : "no");
    }
}
