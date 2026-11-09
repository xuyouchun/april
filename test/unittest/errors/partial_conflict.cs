/* EXPECT_ERROR conflicting */

import System;

using System;

public partial class A
{
    public int X;
}

private partial class A
{
    public int Y;
}

class Program
{
    [EntryPoint]
    public static void Main() { }
}
