/* EXPECT_ERROR a tuple of 3 items cannot be made for type */
/* EXPECT_CODE AP3219 */

// The count of the items of a tuple literal is that of the tuple type that it is made for.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Tuple<Int32, String> t = (1, 2, 3);
    }
}
