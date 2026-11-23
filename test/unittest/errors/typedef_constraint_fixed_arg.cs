/* EXPECT_ERROR must be a non-nullable value type */

import System;

using System;
using System.Collections;

class Box<T> where T : struct { public T v; }

typedef Bad = Box<String>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 1;
    }
}
