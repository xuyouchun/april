/* EXPECT_ERROR not defined */

import System;

using System;

class Util
{
    public static T Add<T>(T a, T b)
    {
        return a + b;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
