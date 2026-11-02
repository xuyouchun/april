/* EXPECT_ERROR can be used only through a generic parameter */

import System;

using System;

interface IZero
{
    static abstract int Zero();
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int x = IZero.Zero();
    }
}
