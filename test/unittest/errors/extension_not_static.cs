/* EXPECT_ERROR an extension method must be a static method of a static class */
/* EXPECT_CODE AP2313 */

import System;

using System;

class Helpers
{
    public static int Twice(this int value)
    {
        return value * 2;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Helpers.Twice(2));
    }
}
