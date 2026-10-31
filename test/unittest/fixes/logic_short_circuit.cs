// && and || do not evaluate the second operand when the first decides the result.

/* EXPECT

a
c
e
f
g
h
i
False
safe
pos
k
m
True
guard

*/

import System;

using System;

class Node
{
    public int V;
}

class Program
{
    static bool T(string s)
    {
        Console.WriteLine(s);
        return true;
    }

    static bool F(string s)
    {
        Console.WriteLine(s);
        return false;
    }

    [EntryPoint]
    public static void Main()
    {
        if (F("a") && F("b")) { }
        if (T("c") || T("d")) { }
        if (T("e") && T("f")) { }
        if (F("g") || T("h")) { }

        bool r = F("i") && T("j");
        Console.WriteLine(r);

        // The second operand dereferences null.
        Node n = null;
        if (n != null && n.V > 0)
            Console.WriteLine("pos");
        else
            Console.WriteLine("safe");

        n = new Node();
        n.V = 5;
        if (n != null && n.V > 0)
            Console.WriteLine("pos");

        // Nested.
        bool x = (T("k") || F("l")) && T("m");
        Console.WriteLine(x);

        // As the condition of a loop.
        int[] a = new int[3];
        int i = 0;
        while (i < 3 && a[i] == 0)
            i++;

        if (i == 3 || a[i] == 0)
            Console.WriteLine("guard");
    }
}
