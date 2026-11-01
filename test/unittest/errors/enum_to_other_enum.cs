/* EXPECT_ERROR Cannot implicitly convert */

// An enum is not converted to another enum.

import System;

using System;

enum Color { Red, Green }

enum Size { Small, Large }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Size s = Size.Small;
        Color c = s;
    }
}
