/* EXPECT_ERROR Incompatible return type */

// An int and a String have no type that both are converted to.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        var a = (int x) => { if (x > 0) { return 1; } return "s"; };
    }
}
