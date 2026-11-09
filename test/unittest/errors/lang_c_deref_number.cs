/* EXPECT_ERROR (Value) */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 5;

        // n is not a pointer: *n is n.Value.
        <%c { Console.WriteLine(*n); } %>
    }
}
