/* EXPECT_ERROR type "Tmp" is not defined */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        {
            typedef Tmp = Int32;
            Tmp t = 1;
        }

        Tmp u = 2;
    }
}
