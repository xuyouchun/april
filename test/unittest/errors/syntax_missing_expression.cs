/* EXPECT_ERROR expected expression */
/* EXPECT_AT 11:44 */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main() { int x = 1 +; }
}
