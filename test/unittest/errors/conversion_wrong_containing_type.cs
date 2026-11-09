/* EXPECT_ERROR must be the containing type */

import System;

using System;

struct M
{
    public Int32 V;

    public static implicit operator Int32(String s) { return 1; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
