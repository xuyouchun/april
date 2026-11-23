/* EXPECT_ERROR "Item2" cannot be the name of an item of a tuple */
/* EXPECT_CODE AP3220 */

// ItemN is the name of the item N only.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        (Int32 Item2, String Str) t = (1, "2");
    }
}
