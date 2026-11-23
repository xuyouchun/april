/* EXPECT_ERROR conflicts with the type */

import System;

using System;
using System.Collections;

class Thing { }

typedef Thing = Int32;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Thing t = new Thing();
    }
}
