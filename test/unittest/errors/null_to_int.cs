/* EXPECT_ERROR Cannot implicitly convert */

// null is converted to references and to T?, not to a number.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int x = null;
    }
}
