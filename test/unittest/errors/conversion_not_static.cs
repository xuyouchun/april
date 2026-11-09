/* EXPECT_ERROR must be declared public and static */

import System;

using System;

struct M
{
    public Int32 V;

    public implicit operator Int32(M m) { return m.V; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
