/* EXPECT_ERROR is a pack: only the last param can be one */

import System;

using System;
using System.Collections;

typedef Bad<T..., U> = Dictionary<T, U>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 1;
    }
}
