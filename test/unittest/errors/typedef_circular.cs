/* EXPECT_ERROR refers to itself */

import System;

using System;
using System.Collections;

typedef A = B;
typedef B = A;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        A a = null;
    }
}
