/* EXPECT_ERROR the operation overflows at compile time */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(checked(100000 * 100000));
    }
}
