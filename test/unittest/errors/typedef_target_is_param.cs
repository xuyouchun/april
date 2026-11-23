/* EXPECT_ERROR cannot be the generic param */

import System;

using System;
using System.Collections;

typedef Same<T> = T;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 1;
    }
}
