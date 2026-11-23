/* EXPECT_ERROR cannot determine the type of item 2 of the tuple */
/* EXPECT_CODE AP3218 */

// An item that is null has no type when the context does not tell it.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        var t = (1, null);
    }
}
