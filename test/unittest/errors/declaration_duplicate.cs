/* EXPECT_ERROR (duplicate|already) */

import System;

using System;

class Program
{
    static void Get(out int value) { value = 1; }

    [EntryPoint]
    public static void Main()
    {
        Get(out int x);
        Get(out int x);
    }
}
