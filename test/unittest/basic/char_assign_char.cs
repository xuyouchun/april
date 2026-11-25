// c += d and c -= d of two Chars: the Char that is added is a number, the result wraps at 16 bits.

/* EXPECT

195
98
65535
1
b
abc

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Char c = 'a';
        Char d = 'b';
        c += d;
        Console.WriteLine((Int32)c);

        c -= 'a';
        Console.WriteLine((Int32)c);

        Char e = (Char)0;
        Char one = (Char)1;
        e -= one;                       // wraps
        Console.WriteLine((Int32)e);
        e += (Char)2;
        Console.WriteLine((Int32)e);

        Char[] letters = new Char[3];
        letters[0] = 'a';
        letters[1] = 'a';
        letters[1] += (Char)1;
        Console.WriteLine(letters[1]);

        Char x = 'a';
        String s = "";
        for (Int32 i = 0; i < 3; i++)
        {
            s += x;
            x += one;
        }

        Console.WriteLine(s);
    }
}
