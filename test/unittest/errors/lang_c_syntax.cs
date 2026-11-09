/* EXPECT_ERROR (expected expression) */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        <%c { int* p = ; } %>
    }
}
