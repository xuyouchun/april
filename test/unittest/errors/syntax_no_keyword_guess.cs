/* EXPECT_ERROR unexpected token "new" */
/* EXPECT_NOT expected "bool" */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int x = 1;
        x new 2;
    }
}
