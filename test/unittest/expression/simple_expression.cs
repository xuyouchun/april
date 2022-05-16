
// Simple expression.

/* EXPECT

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
        int a = 1 + 2 * 3;
        int b = (1 + 2) * 3;

        Console.WriteLine(a);
        Console.WriteLine(b);
    }
}

