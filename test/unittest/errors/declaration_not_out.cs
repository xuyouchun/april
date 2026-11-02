/* EXPECT_ERROR unexpected expression */

import System;

using System;

class Program
{
    static void Take(int value) { }

    [EntryPoint]
    public static void Main()
    {
        Take(int x);
    }
}
