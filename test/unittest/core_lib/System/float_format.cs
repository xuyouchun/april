// Floating point numbers are written like .NET: the shortest text that reads back exactly.

/* EXPECT

0.3333333333333333
100000000
1E+15
100000000000000
1.2345678901234568E+17
0.00001
1E-06
2.5
-0.5
3
0.30000000000000004
1.1
3.3000002
Infinity
-Infinity
NaN

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        double a = 1.0 / 3.0;
        Console.WriteLine(a);
        Console.WriteLine(100000000.0);
        Console.WriteLine(1e15);
        Console.WriteLine(1e14);
        Console.WriteLine(123456789012345678.0);
        Console.WriteLine(0.00001);
        Console.WriteLine(0.000001);
        Console.WriteLine(2.5);
        Console.WriteLine(-0.5);
        Console.WriteLine(3.0);
        Console.WriteLine(0.1 + 0.2);

        float f = 1.1f;
        Console.WriteLine(f);
        Console.WriteLine(f * 3.0f);

        double z = 0.0;
        Console.WriteLine(1.0 / z);
        Console.WriteLine(-1.0 / z);
        Console.WriteLine(z / z);
    }
}
