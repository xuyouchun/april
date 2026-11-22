// A chain of + of strings is joined by one String.Concat: the texts are those of the operator + (null is nothing, numbers, char,
// bool, enums, objects), the order of the evaluation is from the left to the right, and a chain of more than 8 operands is
// joined by groups.

/* EXPECT

a1b2.5c
nulls [] [x] .
kinds 5 a True 7 -3 4000000000 1.5 Red thing 12
order 1 2 3 4
long 1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19
ab12cd
object stays x:5
twice ab ab
done

*/

import System;

using System;

enum Color
{
    Red,
    Green,
}

class Thing : IStringable
{
    public String ToString() => "thing";
}

class Program
{
    static Int32 __counter;

    static Int32 Next()
    {
        __counter = __counter + 1;
        return __counter;
    }

    static String Tag(String text)
    {
        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        String a = "a", b = "b", c = "c";
        Int32 one = 1;
        Double d = 2.5;
        String? none = null;

        Console.WriteLine(a + one + b + d + c);
        Console.WriteLine("nulls [" + none + "] [" + "x" + "] " + none + ".");

        Int8 i8 = 5;
        Char ch = 'a';
        Boolean flag = true;
        UInt8 u8 = 7;
        Int16 i16 = -3;
        UInt32 u32 = 4000000000;
        Single f = 1.5f;
        Object thing = new Thing();
        Int64 big = 12;
        Console.WriteLine("kinds " + i8 + " " + ch + " " + flag + " " + u8 + " " + i16 + " " + u32 + " " + f + " " + Color.Red + " " + thing + " " + big);

        // The operands are evaluated from the left to the right.
        Console.WriteLine("order " + Next() + " " + Next() + " " + Next() + " " + Next());

        String text = "";
        for (Int32 k = 1; k < 20; k++)
            text = text + (k == 1? "" : "-") + k;

        Console.WriteLine("long " + 1 + "-" + 2 + "-" + 3 + "-" + 4 + "-" + 5 + "-" + 6 + "-" + 7 + "-" + 8 + "-" + 9 + "-" + 10 + "-" + 11
                          + "-" + 12 + "-" + 13 + "-" + 14 + "-" + 15 + "-" + 16 + "-" + 17 + "-" + 18 + "-" + 19 + text.Substring(0, 0));

        // Chains inside chains, with parentheses.
        Console.WriteLine(("a" + b) + (one + one * 0 + "")  + ((one + 1) + "") + c + "d");

        Object boxed = 5;
        Console.WriteLine("object stays " + "x" + ":" + boxed);

        String pair = a + b + " " + a + b;
        Console.WriteLine("twice " + pair);
        Console.WriteLine("done");
    }
}
