/* EXPECT_ERROR cannot be any type */

import System;

using System;
using System.Collections;

class Two<A, B> where A : struct where B : class { }

typedef Same<T> = Two<T, T>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 1;
    }
}
