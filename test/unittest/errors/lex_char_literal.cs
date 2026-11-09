/* EXPECT_ERROR too many characters in a character literal */
/* EXPECT_CODE AP0012 */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main() { char c = 'ab'; }
}
