/* EXPECT_ERROR unknown embedded language "nolang" */

import System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 a = 1;
        var b = <%nolang from x in xs select x;
    }
}
