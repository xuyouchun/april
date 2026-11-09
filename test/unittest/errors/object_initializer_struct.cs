/* EXPECT_ERROR cannot be set by an object initializer, only the objects of a class can */

import System;

using System;

struct P
{
    public Int32 X;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        P p = new P() { X = 1 };
    }
}
