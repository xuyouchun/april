/* EXPECT_ERROR variable name is missing */

// The items of a deconstruction that declares variables have names.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        (int, string) = (1, "2");
    }
}
