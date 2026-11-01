// Number literals, and the implicit conversions of numbers.

/* EXPECT

5
1000
31
4294967295
4294967295
170
10.5
1000001
200
205
7
7
65535
2
5

*/

import System;

using System;
class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(0b101);
        Console.WriteLine(1_000);
        int h = 0x1F;
        Console.WriteLine(h);
        long big = 0xFFFF_FFFF;
        Console.WriteLine(big);
        uint u = 0xFFFFFFFF;
        Console.WriteLine(u);
        long bits = 0b1010_1010;
        Console.WriteLine(bits);
        double d = 1_0.5_0;
        Console.WriteLine(d);
        Console.WriteLine(1_000_000 + 1);
        byte b = (byte)200;
        int i = b;
        Console.WriteLine(i);
        short s = (short)5;
        int j = s + b;
        Console.WriteLine(j);
        uint w = (uint)7;
        double wd = w;
        Console.WriteLine(wd);
        long wl = w;
        Console.WriteLine(wl);
        ushort us = (ushort)65535;
        int ui = us;
        Console.WriteLine(ui);
        float f = 2;
        Console.WriteLine(f);
        ulong ul = 5;
        double ud = ul;
        Console.WriteLine(ud);
    }
}
