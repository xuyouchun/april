/* EXPECT_ERROR Cannot implicitly convert */

// An enum is converted to a number by a cast: (int)size.

import System;

using System;

enum Size { Small, Large }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Size s = Size.Large;
        int x = s;
    }
}
