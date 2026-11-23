/* EXPECT_ERROR must be a reference type */

import System;

using System;
using System.Collections;

class Ref<T> where T : class { }

typedef RRef<T> = Ref<T>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        RRef<Int32> b = null;
    }
}
