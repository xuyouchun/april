/* EXPECT_ERROR is not valid for interface members */

import System;

using System;

interface IZero
{
    virtual int Zero() { return 0; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {

    }
}
