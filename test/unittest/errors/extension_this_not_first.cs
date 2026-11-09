/* EXPECT_ERROR the modifier "this" can only be on the first param */
/* EXPECT_CODE AP2312 */

import System;

using System;

static class Helpers
{
    public static int Add(int a, this int b)
    {
        return a + b;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Helpers.Add(1, 2));
    }
}
