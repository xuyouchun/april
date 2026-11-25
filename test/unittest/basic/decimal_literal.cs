// decimal: 1.5m is an exact number, the operations keep the digits: 0.1m + 0.2m is 0.3m. The scale is that of the operands.

/* EXPECT

0.3
true
1.50
3.00
2
0.333333333333333333
7.5
-1.5
compare True False True
int 6 5
round 2 4 2.34
floor -2 ceiling -1
parse 12.34 -5
money 19.98
big 1234567890123.456789
convert 7 2.5
loop 0.5
default 0
text 1.5 1.50

*/

import System;

using System;

class Program
{
    static decimal Price(decimal unit, Int32 count) { return unit * count; }

    [EntryPoint]
    public static void Main()
    {
        decimal a = 0.1m;
        decimal b = 0.2m;
        Console.WriteLine(a + b);
        Console.WriteLine((a + b == 0.3m)? "true" : "false");

        Console.WriteLine(1.5m * 1.0m * 1m);
        Console.WriteLine(1.50m + 1.5m);
        Console.WriteLine(6m / 3m);
        Console.WriteLine(1m / 3m);
        Console.WriteLine(15m / 2m);
        Console.WriteLine(-1.5m);

        Console.WriteLine("compare " + (1.5m > 1.4m) + " " + (1.5m < 1.4m) + " " + (1.50m == 1.5m));

        decimal d = 5;
        d = d + 1;
        Int32 i = (Int32)d;
        Console.WriteLine("int " + d + " " + (i - 1));

        Console.WriteLine("round " + Decimal.Round(2.5m) + " " + Decimal.Round(3.5m) + " " + Decimal.Round(2.345m, 2));
        Console.WriteLine("floor " + Decimal.Floor(-1.5m) + " ceiling " + Decimal.Ceiling(-1.5m));

        Console.WriteLine("parse " + Decimal.Parse("12.34") + " " + Decimal.Parse("-5"));

        Console.WriteLine("money " + Price(9.99m, 2));

        decimal big = 1234567890123.456789m;
        Console.WriteLine("big " + big);

        Console.WriteLine("convert " + (Int32)7.9m + " " + (Double)2.5m);

        decimal sum = 0;
        for (Int32 k = 0; k < 5; k++)
            sum += 0.1m;

        Console.WriteLine("loop " + sum);

        decimal zero = default;
        Console.WriteLine("default " + zero);

        Console.WriteLine("text " + 1.5m + " " + 1.50m);
    }
}
