/* EXPECT_ERROR the type of the lambda is not determined */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int a = 3;
        int b = (() => { return a * 2; })();
    }
}
