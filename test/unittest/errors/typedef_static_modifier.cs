/* EXPECT_ERROR is not valid for a typedef */

import System;

using System;
using System.Collections;

class Holder
{
    static typedef Num = Int32;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 1;
    }
}
