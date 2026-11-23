/* EXPECT_ERROR can only be public or internal */

import System;

using System;
using System.Collections;

namespace N
{
    private typedef Num = Int32;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        N.Num n = 1;
    }
}
