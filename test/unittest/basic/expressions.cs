// Test statements.

/* EXPECT

30
10
200
2

30.3
10.1
204.02
2

True
True
False
True
True
True

False
True
False

0
255
255
-171

7
9

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Simple alghthriams.
        {
            int a = 20, b = 10;
            Console.WriteLine(a + b);
            Console.WriteLine(a - b);
            Console.WriteLine(a * b);
            Console.WriteLine(a / b);
        }

        // Simple float algorhth
        {
            double a = 20.2, b = 10.1;
            Console.WriteLine(a + b);
            Console.WriteLine(a - b);
            Console.WriteLine(a * b);
            Console.WriteLine(a / b);
        }

        // Compare.
        {
            int a = 20, b = 10, c = 20;
            Console.WriteLine(a > b);
            Console.WriteLine(a >= c);
            Console.WriteLine(a < b);
            Console.WriteLine(a <= c);
            Console.WriteLine(a != b);
            Console.WriteLine(a == c);
        }

        // Logic
        {
            int a = 20, b = 10, c = 20;
            Console.WriteLine(a > b && b > c);
            Console.WriteLine(a > b || b > c);
            Console.WriteLine(!(a > b));
        }

        // Bit
        {
            int a = 0B1010101, b = 0B10101010;
            Console.WriteLine(a & b);
            Console.WriteLine(a | b);
            Console.WriteLine(a ^ b);
            Console.WriteLine(~b);
        }

        // Priorities
        {
            Console.WriteLine(1 + 2 * 3);
            Console.WriteLine((1 + 2) * 3);
        }
    }
}


