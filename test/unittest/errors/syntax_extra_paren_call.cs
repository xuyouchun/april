/* EXPECT_ERROR unexpected token "\)" */
/* EXPECT_AT 13:29 */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(1));
    }
}
