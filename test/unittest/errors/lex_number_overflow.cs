/* EXPECT_ERROR the numeric literal is too large */
/* EXPECT_CODE AP0014 */
/* EXPECT_NOT (invalid expression|undefined) */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main() { int x = 99999999999999999999; }
}
