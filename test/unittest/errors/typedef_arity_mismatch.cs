/* EXPECT_ERROR is not defined */

import System;

using System;
using System.Collections;

typedef M<T> = List<T>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        M<String, Int32> m = null;
    }
}
