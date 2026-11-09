/* EXPECT_ERROR unknown embedded language "nolang" */

import System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        var a = <%nolang from x in xs select x %>;
    }
}
