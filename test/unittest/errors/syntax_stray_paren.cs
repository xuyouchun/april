/* EXPECT_ERROR unexpected token "\)" */
/* EXPECT_AT 16:10 */
/* EXPECT_NOT expected "\(" */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        using (new Program())
        {
        })
    }
}
