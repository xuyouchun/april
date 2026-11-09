/* EXPECT_ERROR unknown escape sequence */
/* EXPECT_CODE AP0011 */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main() { string s = "a\qb"; }
}
