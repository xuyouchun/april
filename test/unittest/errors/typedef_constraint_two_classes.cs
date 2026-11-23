/* EXPECT_ERROR no type derives from both */

import System;

using System;
using System.Collections;

class A1 { }
class B1 { }
class Pair<X, Y> where X : A1 where Y : B1 { }

typedef Same<T> = Pair<T, T>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 1;
    }
}
