/* EXPECT_ERROR already defines a member */

import System;

using System;

struct M
{
    public Int32 V;

    public static explicit operator Int32(M m) { return m.V; }

    public static explicit operator Int32(M m) { return m.V + 1; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
