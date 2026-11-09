/* EXPECT_ERROR unterminated string literal */
/* EXPECT_CODE AP0011 */
/* EXPECT_AT 13:44 */
/* EXPECT_NOT unexpected end of file */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main() { string s = "abc; }
}
