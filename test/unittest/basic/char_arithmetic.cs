// Arithmetic of Char: c + 1 and c - 1 are Chars, c - 'a' is an Int32, ++ -- += -= work, the result wraps at 16 bits.

/* EXPECT

b c y 5
b ` c b
c 0
5
5
abc
e 3 4
65534

*/

import System;

using System;

class Program
{
    static Char Next(Char c) { return c + 1; }

    static Char Shift(Char c, Int64 n) { return c + n; }

    static Int32 Digit(Char c) { return c - '0'; }

    [EntryPoint]
    public static void Main()
    {
        Char c = 'a';
        Char d = (Char)(c + 1);
        Char e = c + 2;
        Char f = 'z' - 1;
        Int32 diff = 'f' - 'a';
        Console.WriteLine(d + " " + e + " " + f + " " + diff);

        c++;
        Console.Write(c + " ");
        c--;
        c--;
        Console.Write(c + " ");
        c += 3;
        Console.Write(c + " ");
        c -= 1;
        Console.WriteLine(c);

        Char g = c + (Int64)65537;      // wraps: 65537 is 1 at 16 bits
        Char h = (Char)65535;
        h++;
        Console.WriteLine(g + " " + (Int32)h);

        Console.WriteLine('0' + 5);
        Console.WriteLine((Char)('0' + 5));

        for (Char x = 'a'; x < 'd'; x++)
            Console.Write(x);
        Console.WriteLine();

        Console.WriteLine(Next('d') + " " + Digit('3') + " " + Digit('4'));

        Char big = (Char)1;
        big -= 3;
        Console.WriteLine((Int32)big);
    }
}
