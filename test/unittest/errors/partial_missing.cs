/* EXPECT_ERROR "partial" modifier is missing */

import System;

using System;

partial class A
{
    public int X;
}

class A
{
    public int Y;
}

class Program
{
    [EntryPoint]
    public static void Main() { }
}
