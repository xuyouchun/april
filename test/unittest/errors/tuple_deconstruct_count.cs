/* EXPECT_ERROR member "Item3" is not defined in type */

// A deconstruction has as many variables as the tuple has items.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        (Int32 a, Int32 b, Int32 c) = (1, 2);
    }
}
