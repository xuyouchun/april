/* EXPECT_ERROR interface members cannot have a defination */

import System;

using System;

interface IZero
{
    static abstract int Zero() { return 0; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {

    }
}
