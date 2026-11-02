// Floating point literals with an exponent.

/* EXPECT

1500
1000
0.025
1500
483
12300
0.5
150
7.5

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(1.5e3); // The sign of the exponent is optional.
        Console.WriteLine(1e3);
        Console.WriteLine(2.5e-2);
        Console.WriteLine(1.5E+3);
        Console.WriteLine(0x1e3); // 'e' is a digit of a hexadecimal number.
        Console.WriteLine(123.0e+2);
        Console.WriteLine(5e-1);

        double x = 1.5e2;
        Console.WriteLine(x);
        Console.WriteLine(x / 2e1);
    }
}
