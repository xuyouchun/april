// The null conditional operator: a?.b, a?.F(), a?.b.c, a?.b?.c. When a is null the value is null
// (a value type is nullable then), the rest of the chain is not evaluated.

/* EXPECT

hello
3
True
True
True
3
7
-1
5
-1
2
yes
no
7
0
True
5
x
calls 1
none
4
done

*/

import System;

using System;

class Node
{
    public int N;
    public String S;
    public Node Next;
    public int? Opt;

    public static int Calls;

    public int Len() { return N; }

    public Node Self() { return this; }

    public Node Make()
    {
        Calls++;
        return this;
    }

    public void Say(String s) { Console.WriteLine(s); }

    public String Name { get { return S; } }
}

class Program
{
    static int Count(Node n)
    {
        return n?.N ?? -1;
    }

    static String Describe(Node n)
    {
        return n?.S;
    }

    [EntryPoint]
    public static void Main()
    {
        Node b = new Node();
        b.N = 3;
        b.S = "hello";
        b.Next = null;
        Node nb = null;

        String s = b?.S;
        Console.WriteLine(s);

        int? n = b?.N;
        Console.WriteLine(n.Value);

        int? m = nb?.N;
        Console.WriteLine(m == null);
        Console.WriteLine(nb?.S == null);
        Console.WriteLine(b?.Next?.S == null);

        // A chain: the methods of the objects.
        Console.WriteLine(b?.Self().N ?? 7);
        Console.WriteLine(nb?.Self().N ?? 7);
        Console.WriteLine(nb?.Len() ?? -1);
        Console.WriteLine(b?.S.Length ?? -1);

        // A function that takes it, and returns it.
        Console.WriteLine(Count(nb));
        Console.WriteLine(Count(b) - 1);

        if ((b?.N ?? 0) > 1)
            Console.WriteLine("yes");

        if ((nb?.N ?? 0) > 1)
            Console.WriteLine("never");
        else
            Console.WriteLine("no");

        // A nullable member.
        b.Opt = 7;
        Console.WriteLine(b?.Opt ?? 0);
        Console.WriteLine(nb?.Opt ?? 0);

        // A property, and a string.
        Console.WriteLine(Describe(b) == "hello");
        String text = "abcde";
        String none = null;
        Console.WriteLine(text?.Length ?? -1);

        // A void method is called when the object is not null.
        nb?.Say("never");
        b?.Say("x");

        // The rest of the chain is not evaluated when the object is null.
        Node.Calls = 0;
        nb?.Make().Make();
        b?.Make().Make();
        Console.WriteLine("calls " + (Node.Calls - 1));

        Console.WriteLine(none?.Length == null? "none" : "some");
        Console.WriteLine(Describe(nb) == null? 4 : 5);
        Console.WriteLine("done");
    }
}
