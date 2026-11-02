/* EXPECT_ERROR does not implement interface member */

import System;

using System;

interface IZero
{
    static abstract int Zero();
}

class A : IZero
{
    public int Zero() { return 0; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {

    }
}
