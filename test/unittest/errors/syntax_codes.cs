/* EXPECT_ERROR unexpected token "\)" */
/* EXPECT_CODE AP0002 */

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
