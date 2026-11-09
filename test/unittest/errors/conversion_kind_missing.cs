/* EXPECT_ERROR must be declared implicit or explicit */

import System;

using System;

struct M
{
    public Int32 V;

    public static operator Int32(M m) { return m.V; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
