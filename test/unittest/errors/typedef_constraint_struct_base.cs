/* EXPECT_ERROR must be a value type */

import System;

using System;
using System.Collections;

class A1 { }
class Gen<X, Y> where X : struct where Y : A1 { }

typedef Same<T> = Gen<T, T>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 1;
    }
}
