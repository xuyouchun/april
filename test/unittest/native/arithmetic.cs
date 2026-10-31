// Arithmetic, bit and compare operators on primitive values.

/* EXPECT

22
12
85
3
2
-17
-3
-2
10000000000
300000
33333
1333333333
1
True

15
3.75
5.5
True

False

8
14
6
-13
48
3
-3
True

True

False

False

False

True

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Int32.
        {
            int a = 17, b = 5;
            Console.WriteLine(a + b);
            Console.WriteLine(a - b);
            Console.WriteLine(a * b);
            Console.WriteLine(a / b);
            Console.WriteLine(a % b);
            Console.WriteLine(-a);
            Console.WriteLine(-17 / 5);
            Console.WriteLine(-17 % 5);
        }

        // Int64 and mixed sizes.
        {
            long x = 100000;
            int y = 3;
            Console.WriteLine(x * x);
            Console.WriteLine(x * y);
            Console.WriteLine(x / y);
        }

        // Unsigned.
        {
            uint a = (uint)4000000000;
            uint b = (uint)3;
            Console.WriteLine(a / b);
            Console.WriteLine(a % b);
            Console.WriteLine(a > b);
        }

        // Floats.
        {
            double d = 7.5;
            double e = 2.0;
            Console.WriteLine(d * e);
            Console.WriteLine(d / e);
            Console.WriteLine(d - e);
            Console.WriteLine(d > e);
            Console.WriteLine(d == e);
        }

        // Bit operators.
        {
            int a = 12, b = 10;
            Console.WriteLine(a & b);
            Console.WriteLine(a | b);
            Console.WriteLine(a ^ b);
            Console.WriteLine(~a);
            Console.WriteLine(a << 2);
            Console.WriteLine(a >> 2);
            Console.WriteLine(-a >> 2);
        }

        // Compare.
        {
            int a = 3, b = 4;
            Console.WriteLine(a < b);
            Console.WriteLine(a <= b);
            Console.WriteLine(a > b);
            Console.WriteLine(a >= b);
            Console.WriteLine(a == b);
            Console.WriteLine(a != b);
        }
    }
}
