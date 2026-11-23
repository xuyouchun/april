/* EXPECT_ERROR must be a non-nullable value type */

import System;

using System;
using System.Collections;

class Box<T> where T : struct { public T v; }

typedef SBox<T> = Box<T>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        SBox<String> b = null;
    }
}
