/* EXPECT_ERROR cannot implicitly convert */

import System;

using System;

struct Point
{
    public int X;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Point p = null;
    }
}
