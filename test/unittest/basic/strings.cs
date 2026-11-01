// Strings: concatenation, comparison, conversion of values to strings.

/* EXPECT

abcd
True
True
False
x1
1x
a2.5b
True!
c
4
True
False
True
hello world
1
abc
Value: 7
True
False
True
ab1
01234
20

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        String a = "ab";
        String b = "cd";
        String c = a + b;
        Console.WriteLine(c);

        Console.WriteLine(a == "ab");
        Console.WriteLine(a != b);
        Console.WriteLine(a == b);

        // Values are converted.
        Console.WriteLine("x" + 1);
        Console.WriteLine(1 + "x");
        Console.WriteLine("a" + 2.5 + "b");
        Console.WriteLine(true + "!");
        Console.WriteLine("" + 'c');

        // Length of text built at run time.
        Console.WriteLine(c.Length);

        // Order.
        Console.WriteLine("a" < "b");
        Console.WriteLine("b" < "a");
        Console.WriteLine("abc" <= "abc");

        // Chains and variables.
        String s = "hello";
        s = s + " " + "world";
        Console.WriteLine(s);

        // Compared with null: references.
        String n = null;
        Console.WriteLine(n == null ? 1 : 0 + 3);
        Console.WriteLine("ab" + "c");

        int v = 7;
        Console.WriteLine("Value: " + v);

        Console.WriteLine(c == "ab" + "cd");
        Console.WriteLine(n != null);
        Console.WriteLine(s != null);

        // Compound assignment, strings made in loops.
        String u = "a";
        u += "b";
        u += 1;
        Console.WriteLine(u);

        String t = "";
        for (int i = 0; i < 5; i++)
            t = t + i;
        Console.WriteLine(t);

        String long_text = "";
        for (int i = 0; i < 200; i++)
            long_text += "xy";
        Console.WriteLine(long_text.Length / 20);
    }
}
