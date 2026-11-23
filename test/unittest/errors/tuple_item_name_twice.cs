/* EXPECT_ERROR "Number" cannot be the name of an item of a tuple */
/* EXPECT_CODE AP3220 */

// The names of the items of a tuple are different.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        (Int32 Number, String Number) t = (1, "2");
    }
}
