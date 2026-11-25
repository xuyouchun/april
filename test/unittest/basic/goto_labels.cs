// A labeled statement is where goto label; jumps to, in the method that has it: forward, backward, out of loops. goto case and
// goto default; jump to a case of the switch.

/* EXPECT

count 3
out 2
1 case 2 reached
default reached
string b c
nested 12
done

*/

import System;

using System;

class Program
{
    static String Cases(Int32 k)
    {
        String r = "";

        switch (k)
        {
            case 1:
                r += "1 ";
                goto case 2;

            case 2:
                r += "case 2 reached";
                break;

            case 3:
                goto default;

            default:
                r += "default reached";
                break;
        }

        return r;
    }

    static String Strings(String s)
    {
        String r = "";

        switch (s)
        {
            case "a":
                r += "a ";
                goto case "b";

            case "b":
                r += "b ";
                goto case "c";

            case "c":
                r += "c";
                break;
        }

        return r;
    }

    [EntryPoint]
    public static void Main()
    {
        // Backward.
        Int32 i = 0;
    again:
        i++;
        if (i < 3)
            goto again;

        Console.WriteLine("count " + i);

        // Out of two loops.
        Int32 found = 0;
        for (Int32 a = 0; a < 5; a++)
        {
            for (Int32 b = 0; b < 5; b++)
            {
                if (a * b == 2)
                {
                    found = a * b;
                    goto leave;
                }
            }
        }

    leave:
        Console.WriteLine("out " + found);

        Console.WriteLine(Cases(1));
        Console.WriteLine(Cases(3));
        Console.WriteLine("string " + Strings("b"));

        // Forward, over statements.
        Int32 n = 1;
        goto skip;
        n = 100;

    skip:
        Int32 m = n + 1;
        Console.WriteLine("nested " + n + m);

        Console.WriteLine("done");
    }
}
