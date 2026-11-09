/* EXPECT_ERROR (duplicate|already) */

import System;

using System;

partial class A
{
    public int F() { return 1; }
}

partial class A
{
    public int F() { return 2; }
}

class Program
{
    [EntryPoint]
    public static void Main() { }
}
