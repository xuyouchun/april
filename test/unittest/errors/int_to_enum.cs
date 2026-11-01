/* EXPECT_ERROR Cannot implicitly convert */

// A number is converted to an enum by a cast: (Size)1.

import System;

using System;

enum Size { Small, Large }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int x = 1;
        Size s = x;
    }
}
