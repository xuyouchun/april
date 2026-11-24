// A switch of strings: the cases are compared with ==, in the order that they are written; a case can have many labels,
// a section that has no break goes on in the next one (as the other switches do), null is a case.

/* EXPECT

a one
b two
c two
other three
null
empty
three 3
same x
nested 1 b
loop a b c ?
const-like one
return big
default only
no default
fall a-b-c
fall b-c
fall z
after

*/

import System;

using System;

class Program
{
    static String Name(String s)
    {
        switch (s)
        {
            case "a":
                return "one";

            case "b":
            case "c":
                return "two";

            case null:
                return "null";

            case "":
                return "empty";

            default:
                return "three";
        }
    }

    static Int32 Size(String s)
    {
        switch (s)
        {
            case "x": return 1;
            case "xy": return 2;
            case "xyz": return 3;
        }

        return 0;
    }

    static String Pick(String s)
    {
        String r = "?";

        switch (s)
        {
            case "a":
                r = "a";
                break;

            case "b":
                r = "b";
                break;

            case "c":
                r = "c";
                break;
        }

        return r;
    }

    static String Fall(String s)
    {
        String r = "";

        switch (s)
        {
            case "a":
                r += "a-";

            case "b":
                r += "b-";

            case "c":
                r += "c";
                break;

            default:
                r += "z";
                break;
        }

        return r;
    }

    static String Big(String s)
    {
        switch (s)
        {
            case "k0": return "zero"; case "k1": return "one"; case "k2": return "two"; case "k3": return "three";
            case "k4": return "four"; case "k5": return "five"; case "k6": return "six"; case "k7": return "seven";
            case "k8": return "eight"; case "k9": return "nine"; case "big": return "big";
        }

        return "none";
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("a " + Name("a"));
        Console.WriteLine("b " + Name("b"));
        Console.WriteLine("c " + Name("c"));
        Console.WriteLine("other " + Name("zzz"));
        Console.WriteLine(Name(null));
        Console.WriteLine(Name(""));
        Console.WriteLine("three " + Size("xyz"));

        // The value is evaluated once.
        Int32 calls = 0;
        String same = "x";
        switch (same + (calls++ == 0? "" : "!"))
        {
            case "x":
                Console.WriteLine("same " + same);
                break;

            default:
                Console.WriteLine("same ?");
                break;
        }

        // A switch in a switch.
        String outer = "p", inner = "b";
        switch (outer)
        {
            case "p":
                switch (inner)
                {
                    case "a": Console.WriteLine("nested a"); break;
                    case "b": Console.WriteLine("nested " + calls + " b"); break;
                }
                break;
        }

        // A switch in a loop: break leaves the switch, continue goes on in the loop.
        String line = "loop";
        foreach (String item in new String[] { "a", "b", "skip", "c", "d" })
        {
            switch (item)
            {
                case "skip":
                    continue;

                case "d":
                    break;

                default:
                    line += " " + Pick(item);
                    break;
            }
        }

        Console.WriteLine(line + " " + Pick("zz"));

        const String One = "one";
        switch (One)
        {
            case "one": Console.WriteLine("const-like one"); break;
        }

        Console.WriteLine("return " + Big("big"));

        switch ("q")
        {
            default:
                Console.WriteLine("default only");
                break;
        }

        switch ("q")
        {
            case "a":
                Console.WriteLine("a?");
                break;
        }

        Console.WriteLine("no default");

        Console.WriteLine("fall " + Fall("a"));
        Console.WriteLine("fall " + Fall("b"));
        Console.WriteLine("fall " + Fall("?"));

        Console.WriteLine("after");
    }
}
